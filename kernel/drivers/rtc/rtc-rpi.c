// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
/**
 * rtc-rpi.c
 *
 * RTC driver using firmware mailbox
 * Supports battery backed RTC and wake alarms
 *
 * Based on rtc-meson-vrtc by Neil Armstrong
 *
 * Copyright (c) 2023, Raspberry Pi Ltd.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/rtc.h>
#include <linux/of.h>
#include <soc/bcm2835/raspberrypi-firmware.h>

struct rpi_rtc_data {
	struct rtc_device *rtc;
	struct rpi_firmware *fw;
	u32 bbat_vchg_microvolts;
};

#define RPI_FIRMWARE_GET_RTC_REG 0x00030087
#define RPI_FIRMWARE_SET_RTC_REG 0x00038087

enum {
	RTC_TIME,
	RTC_ALARM,
	RTC_ALARM_PENDING,
	RTC_ALARM_ENABLE,
	RTC_BBAT_CHG_VOLTS,
	RTC_BBAT_CHG_VOLTS_MIN,
	RTC_BBAT_CHG_VOLTS_MAX,
	RTC_BBAT_VOLTS
};

static int rpi_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	struct rpi_rtc_data *vrtc = dev_get_drvdata(dev);
	u32 data[2] = {RTC_TIME};
	int err;

	err = rpi_firmware_property(vrtc->fw, RPI_FIRMWARE_GET_RTC_REG,
				    &data, sizeof(data));
	rtc_time64_to_tm(data[1], tm);
	return err;
}

static int rpi_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	struct rpi_rtc_data *vrtc = dev_get_drvdata(dev);
	u32 data[2] = {RTC_TIME, rtc_tm_to_time64(tm)};

	return rpi_firmware_property(vrtc->fw, RPI_FIRMWARE_SET_RTC_REG,
				     &data, sizeof(data));
}

static int rpi_rtc_alarm_irq_is_enabled(struct device *dev, unsigned char *enabled)
{
	struct rpi_rtc_data *vrtc = dev_get_drvdata(dev);
	u32 data[2] = {RTC_ALARM_ENABLE};
	s32 err = 0;

	err = rpi_firmware_property(vrtc->fw, RPI_FIRMWARE_GET_RTC_REG,
				    &data, sizeof(data));
	*enabled = data[1] & 0x1;
	return err;
}

static int rpi_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	struct rpi_rtc_data *vrtc = dev_get_drvdata(dev);
	u32 data[2] = {RTC_ALARM_ENABLE, enabled};

	return rpi_firmware_property(vrtc->fw, RPI_FIRMWARE_SET_RTC_REG,
				     &data, sizeof(data));
}

static int rpi_rtc_alarm_clear_pending(struct device *dev)
{
	struct rpi_rtc_data *vrtc = dev_get_drvdata(dev);
	u32 data[2] = {RTC_ALARM_PENDING, 1};

	return rpi_firmware_property(vrtc->fw, RPI_FIRMWARE_SET_RTC_REG,
				     &data, sizeof(data));
}

static int rpi_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	struct rpi_rtc_data *vrtc = dev_get_drvdata(dev);
	u32 data[2] = {RTC_ALARM};
	s32 err = 0;

	err = rpi_rtc_alarm_irq_is_enabled(dev, &alarm->enabled);
	if (!err)
		err = rpi_firmware_property(vrtc->fw, RPI_FIRMWARE_GET_RTC_REG,
					    &data, sizeof(data));
	rtc_time64_to_tm(data[1], &alarm->time);

	return err;
}

static int rpi_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	struct rpi_rtc_data *vrtc = dev_get_drvdata(dev);
	u32 data[2] = {RTC_ALARM, rtc_tm_to_time64(&alarm->time)};
	int err;

	err = rpi_firmware_property(vrtc->fw, RPI_FIRMWARE_SET_RTC_REG,
				    &data, sizeof(data));

	if (err == 0)
		err = rpi_rtc_alarm_irq_enable(dev, alarm->enabled);

	return err;
}

static const struct rtc_class_ops rpi_rtc_ops = {
	.read_time = rpi_rtc_read_time,
	.set_time = rpi_rtc_set_time,
	.read_alarm = rpi_rtc_read_alarm,
	.set_alarm = rpi_rtc_set_alarm,
	.alarm_irq_enable = rpi_rtc_alarm_irq_enable,
};

static int rpi_rtc_set_charge_voltage(struct device *dev)
{
	struct rpi_rtc_data *vrtc = dev_get_drvdata(dev);
	u32 data[2] = {RTC_BBAT_CHG_VOLTS, vrtc->bbat_vchg_microvolts};
	int err;

	err = rpi_firmware_property(vrtc->fw, RPI_FIRMWARE_SET_RTC_REG,
				    &data, sizeof(data));

	if (err)
		dev_err(dev, "failed to set trickle charge voltage to %uuV: %d\n",
			vrtc->bbat_vchg_microvolts, err);
	else if (vrtc->bbat_vchg_microvolts)
		dev_info(dev, "trickle charging enabled at %uuV\n",
			 vrtc->bbat_vchg_microvolts);

	return err;
}

static ssize_t rpi_rtc_print_uint_reg(struct device *dev, char *buf, u32 reg)
{
	struct rpi_rtc_data *vrtc = dev_get_drvdata(dev->parent);
	u32 data[2] = {reg, 0};
	int ret = 0;

	ret = rpi_firmware_property(vrtc->fw, RPI_FIRMWARE_GET_RTC_REG,
				    &data, sizeof(data));
	if (ret < 0)
		return ret;

	return sprintf(buf, "%u\n", data[1]);
}

static ssize_t charging_voltage_show(struct device *dev,
				     struct device_attribute *attr,
				     char *buf)
{
	return rpi_rtc_print_uint_reg(dev, buf, RTC_BBAT_CHG_VOLTS);
}
static DEVICE_ATTR_RO(charging_voltage);

static ssize_t charging_voltage_min_show(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	return rpi_rtc_print_uint_reg(dev, buf, RTC_BBAT_CHG_VOLTS_MIN);
}
static DEVICE_ATTR_RO(charging_voltage_min);

static ssize_t charging_voltage_max_show(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	return rpi_rtc_print_uint_reg(dev, buf, RTC_BBAT_CHG_VOLTS_MAX);
}
static DEVICE_ATTR_RO(charging_voltage_max);

static ssize_t battery_voltage_show(struct device *dev,
				    struct device_attribute *attr,
				    char *buf)
{
	return rpi_rtc_print_uint_reg(dev, buf, RTC_BBAT_VOLTS);
}
static DEVICE_ATTR_RO(battery_voltage);

static struct attribute *rpi_rtc_attrs[] = {
	&dev_attr_charging_voltage.attr,
	&dev_attr_charging_voltage_min.attr,
	&dev_attr_charging_voltage_max.attr,
	&dev_attr_battery_voltage.attr,
	NULL
};

static const struct attribute_group rpi_rtc_sysfs_files = {
	.attrs = rpi_rtc_attrs,
};

static int rpi_rtc_probe(struct platform_device *pdev)
{
	struct rpi_rtc_data *vrtc;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct device_node *fw_node;
	struct rpi_firmware *fw;
	int ret;

	fw_node = of_parse_phandle(np, "firmware", 0);
	if (!fw_node) {
		dev_err(dev, "Missing firmware node\n");
		return -ENOENT;
	}

	fw = rpi_firmware_get(fw_node);
	if (!fw)
		return -EPROBE_DEFER;

	vrtc = devm_kzalloc(&pdev->dev, sizeof(*vrtc), GFP_KERNEL);
	if (!vrtc)
		return -ENOMEM;

	vrtc->fw = fw;

	device_init_wakeup(&pdev->dev, 1);

	platform_set_drvdata(pdev, vrtc);

	vrtc->rtc = devm_rtc_allocate_device(&pdev->dev);
	if (IS_ERR(vrtc->rtc))
		return PTR_ERR(vrtc->rtc);

	set_bit(RTC_FEATURE_ALARM_WAKEUP_ONLY, vrtc->rtc->features);
	clear_bit(RTC_FEATURE_UPDATE_INTERRUPT, vrtc->rtc->features);

	vrtc->rtc->ops = &rpi_rtc_ops;
	ret = rtc_add_group(vrtc->rtc, &rpi_rtc_sysfs_files);
	if (ret)
		return ret;

	rpi_rtc_alarm_clear_pending(dev);

	/*
	 * Optionally enable trickle charging - if the property isn't
	 * present (or set to zero), trickle charging is disabled.
	 */
	of_property_read_u32(np, "trickle-charge-microvolt",
			     &vrtc->bbat_vchg_microvolts);

	rpi_rtc_set_charge_voltage(dev);

	return devm_rtc_register_device(vrtc->rtc);
}

static const struct of_device_id rpi_rtc_dt_match[] = {
	{ .compatible = "raspberrypi,rpi-rtc"},
	{},
};
MODULE_DEVICE_TABLE(of, rpi_rtc_dt_match);

static struct platform_driver rpi_rtc_driver = {
	.probe = rpi_rtc_probe,
	.driver = {
		.name = "rpi-rtc",
		.of_match_table = rpi_rtc_dt_match,
	},
};

module_platform_driver(rpi_rtc_driver);

MODULE_DESCRIPTION("Raspberry Pi RTC driver");
MODULE_LICENSE("GPL");
