/*******************************************************************************
* Copyright (c)  2014- 2014  Guangdong OPPO Mobile Telecommunications Corp., Ltd
* VENDOR_EDIT
* Description: Source file for CBufferList.
*           To allocate and free memory block safely.
* Version   : 0.0
* Date      : 2014-07-30
* Author    : Lijiada @Bsp.charge
* ---------------------------------- Revision History: -------------------------
* <version>           <date>          < author >              <desc>
* Revision 0.0        2014-07-30      Lijiada @Bsp.charge
* Modified to be suitable to the new coding rules in all functions.
*******************************************************************************/

#define OPPO_BMS_PAR
#include "oppo_inc.h"

int opchg_get_prop_fast_chg_started(struct opchg_charger *chip)
{
	if (qpnp_batt_gauge && qpnp_batt_gauge->fast_chg_started)
		return qpnp_batt_gauge->fast_chg_started();
	else {
		pr_err("qpnp-charger no batt gauge assuming false\n");
		return false;
	}
}

int opchg_get_prop_fast_chg_allow(struct opchg_charger *chip)
{
	if (qpnp_batt_gauge && qpnp_batt_gauge->get_fast_chg_allow)
		return qpnp_batt_gauge->get_fast_chg_allow();
	else {
		pr_err("qpnp-charger no batt gauge assuming false\n");
		return false;
	}
}

int opchg_set_fast_chg_allow(struct opchg_charger *chip,int enable)
{
	if (qpnp_batt_gauge && qpnp_batt_gauge->set_fast_chg_allow)
		return qpnp_batt_gauge->set_fast_chg_allow(enable);
	else {
		pr_err("qpnp-charger no batt gauge assuming false\n");
		return false;
	}
}

int opchg_set_fast_switch_to_normal_false(struct opchg_charger *chip)
{
	if (qpnp_batt_gauge && qpnp_batt_gauge->set_switch_to_noraml_false)
		return qpnp_batt_gauge->set_switch_to_noraml_false();
	else {
		pr_err("qpnp-charger no batt gauge assuming false\n");
		return false;
	}
}

int opchg_set_fast_normal_to_warm_false(struct opchg_charger *chip)
{
	if (qpnp_batt_gauge && qpnp_batt_gauge->set_normal_to_warm_false)
		return qpnp_batt_gauge->set_normal_to_warm_false();
	else {
		pr_err("qpnp-charger no batt gauge assuming false\n");
		return false;
	}
}

int opchg_get_fast_normal_to_warm(struct opchg_charger *chip)
{
	if (qpnp_batt_gauge && qpnp_batt_gauge->fast_normal_to_warm)
		return qpnp_batt_gauge->fast_normal_to_warm();
	else {
		pr_err("qpnp-charger no batt gauge assuming false\n");
		return false;
	}
}

int opchg_get_fast_low_temp_full(struct opchg_charger *chip)
{
	if (qpnp_batt_gauge && qpnp_batt_gauge->get_fast_low_temp_full)
		return qpnp_batt_gauge->get_fast_low_temp_full();
	else {
		pr_err("qpnp-charger no batt gauge assuming false\n");
		return false;
	}
}
int opchg_get_prop_fast_switch_to_normal(struct opchg_charger *chip)
{
	if (qpnp_batt_gauge && qpnp_batt_gauge->fast_switch_to_normal)
		return qpnp_batt_gauge->fast_switch_to_normal();
	else {
		pr_err("qpnp-charger no batt gauge assuming false\n");
		return false;
	}
}
int opchg_get_prop_fast_normal_to_warm(struct opchg_charger *chip)
{
	if (qpnp_batt_gauge && qpnp_batt_gauge->fast_normal_to_warm)
		return qpnp_batt_gauge->fast_normal_to_warm();
	else {
		pr_err("qpnp-charger no batt gauge assuming false\n");
		return false;
	}
}

int opchg_get_fast_chg_ing(struct opchg_charger *chip)
{
	if (qpnp_batt_gauge && qpnp_batt_gauge->get_fast_chg_ing)
		return qpnp_batt_gauge->get_fast_chg_ing();
	else {
		pr_err("qpnp-charger no batt gauge assuming false\n");
		return false;
	}
}

int opchg_get_prop_authenticate(struct opchg_charger *chip)
{
	if (qpnp_batt_gauge && qpnp_batt_gauge->is_battery_authenticated)
		return qpnp_batt_gauge->is_battery_authenticated();
	else {
		pr_err("qpnp-charger no batt gauge assuming false\n");
		return false;
	}
}

static bool opchg_soc_reduce_slowly_when_1(struct opchg_charger *chip)
{
	static int reduce_count = 0;
	int batt_vol = 0;

	if(opchg_get_prop_batt_present(chip) == false)
		return 0;
	
	batt_vol = opchg_get_prop_battery_voltage_now(chip)/1000;
	if(batt_vol < 3400)
		reduce_count++;
	else 
		reduce_count = 0;
	pr_err("%s batt_vol:%d,reduce_count:%d\n",__func__,batt_vol,reduce_count);
	if(reduce_count < 5){
		return 0;
	} else {
		reduce_count = 5;
		return 1;
	}
}

int opchg_get_prop_batt_capacity_from_bms(struct opchg_charger *chip)
{
	static char is_pon_on = 0;
	static char sync_up_count = 0;
	static char sync_down_count = 0;
	static char sync_down_limit = 0;
	union power_supply_propval ret = {0, }; 
	struct rtc_time	soc_update_rtc_time;
	int rc = 0;

	if(!chip->bms_psy){
		return OPCHG_DEFAULT_BATT_CAPACITY;
	} else if (chip->bms_psy) {
		chip->bms_psy->get_property(chip->bms_psy,POWER_SUPPLY_PROP_CAPACITY, &ret);		
	}
	chip->soc_bms = ret.intval;
		
	if(is_pon_on == 0){
		is_pon_on = 1;
		chip->bat_volt_check_point = chip->soc_bms;
		pr_err("soc is %d after pon\n",chip->bat_volt_check_point);
		return chip->bat_volt_check_point;
	}	
	
	#if 0
	if((chip->chg_present) && (chip->batt_full) && (chip->batt_authen) && (opchg_get_prop_batt_present(chip) == 1) 
		&& (opchg_battery_temp_region_get(chip) == CV_BATTERY_TEMP_REGION__NORMAL 
				|| opchg_battery_temp_region_get(chip) == CV_BATTERY_TEMP_REGION__LITTLE_COOL))
	#else
	if((chip->chg_present) && (chip->batt_full) && (chip->batt_authen) && (opchg_get_prop_batt_present(chip) == 1) 
		&& ((chip->charging_opchg_temp_statu == OPCHG_CHG_TEMP_NORMAL)|| 
		(chip->charging_opchg_temp_statu  == OPCHG_CHG_TEMP_PRE_NORMAL)))
	#endif
	{
		sync_down_count = 0;
		if(sync_up_count >= OPCHG_SOC_CHANGE_60S){
			sync_up_count = 0;
			chip->bat_volt_check_point++;
		} else {
			sync_up_count++;   
		}
			
		if(chip->bat_volt_check_point >= 100){
			chip->bat_volt_check_point = 100;
			chip->batt_pre_full = 1;
		}
		chip->ocv_uv = opchg_backup_ocv_soc(chip->bat_volt_check_point);
		//pr_debug("full,soc_bms:%d,soc_cal:%d,sync_up_count:%d\n",chip->soc_bms,chip->bat_volt_check_point,sync_up_count);
		return chip->bat_volt_check_point;
	} else if((chip->chg_present) && (opchg_get_prop_batt_status(chip) == POWER_SUPPLY_STATUS_CHARGING) && (opchg_get_prop_batt_present(chip) == 1)){
		sync_down_count = 0;
		if(chip->soc_bms == chip->bat_volt_check_point){
			//pr_debug("charing,soc_bms:%d,soc_cal:%d\n",chip->soc_bms,chip->bat_volt_check_point);
		} else if (chip->soc_bms > chip->bat_volt_check_point){		
			if(sync_up_count >= OPCHG_SOC_CHANGE_35S){
				sync_up_count = 0;
				chip->bat_volt_check_point++;				
			} else {
				sync_up_count++;   
			}			
			//pr_debug("charging,soc_bms:%d,soc_cal:%d,sync_up_count:%d\n",chip->soc_bms,chip->bat_volt_check_point,sync_up_count);
		}		
	} else {
		sync_up_count = 0;
		if (chip->soc_bms < chip->bat_volt_check_point){
			if(atomic_read(&chip->bms_suspended) == 1){
				rc = msmrtc_alarm_read_time(&soc_update_rtc_time);
				if (rc < 0) {
					pr_err("%s: failed to read soc update time\n", __func__);
				}
				rtc_tm_to_time(&soc_update_rtc_time, &chip->soc_update_time);
				if((chip->soc_update_time - chip->soc_update_pre_time) >= EIGHT_MINUTES)		//if soc don't update for 8min
					chip->bat_volt_check_point--;
			} 
			
			if(chip->bat_volt_check_point == 100)
				sync_down_limit = OPCHG_SOC_CHANGE_300S;
			else if(chip->bat_volt_check_point >= 95)
				sync_down_limit = OPCHG_SOC_CHANGE_150S;
			else if(chip->bat_volt_check_point >= 60)
				sync_down_limit = OPCHG_SOC_CHANGE_60S;
			else
				sync_down_limit = OPCHG_SOC_CHANGE_40S;
			
			if((opchg_get_prop_battery_voltage_now(chip) < 3300 * 1000) && (opchg_get_prop_batt_present(chip) == true)){
				if(opchg_get_prop_battery_voltage_now(chip) < 3300 * 1000)
					sync_down_limit = OPCHG_SOC_CHANGE_20S;
			} 
			
			if(sync_down_count >= sync_down_limit){
				sync_down_count = 0;
				if(chip->bat_volt_check_point > 1)
					chip->bat_volt_check_point--;
			} else {
				sync_down_count++;
			}			
		}
		//pr_debug("discharging soc_bms:%d,soc_cal:%d,sync_down_count:%d\n",chip->soc_bms,chip->bat_volt_check_point,sync_down_count);
	}	

	if(chip->bat_volt_check_point >= 100){
		chip->bat_volt_check_point = 100; 				
	} else if(chip->bat_volt_check_point < 2){
		if(opchg_soc_reduce_slowly_when_1(chip))
			chip->bat_volt_check_point = 0;
		else
			chip->bat_volt_check_point = 1;
	}
			
	if(chip->bat_volt_check_point <= 2){
		chip->ocv_uv = opchg_backup_ocv_soc(2);
	} else {
#if 1
		if(chip->bat_volt_check_point == 100 && chip->soc_bms > 90)
			chip->ocv_uv = opchg_backup_ocv_soc(100);
		else
			chip->ocv_uv = opchg_backup_ocv_soc(chip->bat_volt_check_point - 1);
#else
		chip->ocv_uv = opchg_backup_ocv_soc(chip->bat_volt_check_point);
#endif
	}

	rc = msmrtc_alarm_read_time(&soc_update_rtc_time);
	if (rc < 0) {
		pr_err("%s: failed to read soc update pre time\n", __func__);
	} else {
		rtc_tm_to_time(&soc_update_rtc_time, &chip->soc_update_pre_time);
	}
	
	return chip->bat_volt_check_point;
}	

int opchg_get_prop_batt_capacity(struct opchg_charger *chip)
{
	int soc;

	if(is_project(OPPO_14043) || is_project(OPPO_14037) || is_project(OPPO_14051) 
		|| is_project(OPPO_15005) || is_project(OPPO_15057) || is_project(OPPO_15025))
		soc = opchg_get_prop_batt_capacity_from_bms(chip);
	else {
		if (qpnp_batt_gauge && qpnp_batt_gauge->get_battery_soc){
			soc = qpnp_batt_gauge->get_battery_soc();
		} else {
			pr_err("qpnp-charger no batt gauge assuming 50percent\n");
			soc = 50;
		}
	}
	
	return soc;
}

int opchg_get_prop_current_now(struct opchg_charger *chip)
{
	int rc = 0;
	struct qpnp_vadc_result results;
	int chg_current = 0;

	if(is_project(OPPO_14043) || is_project(OPPO_15005) || is_project(OPPO_15025)){
		if(!chip->chg_present){
			chg_current = 0;
		} else {	
			rc = qpnp_vadc_read(chip->vadc_dev, P_MUX2_1_1, &results);
			if (rc) {
				pr_err("Unable to read iadc rc=%d\n", rc);
				chg_current = 0;
			}
			chg_current = (int)results.physical/1000;
			chg_current = -(chg_current * 5)/4;
		}
	} else if(is_project(OPPO_14037) || is_project(OPPO_14051) || is_project(OPPO_15057)){
		if(!chip->chg_present){
			chg_current = 0;
		} else {
			rc = qpnp_vadc_read(chip->vadc_dev, P_MUX2_1_3, &results);
			if (rc) {
				pr_err("Unable to read iadc rc=%d\n", rc);
				chg_current = 0;
			}
			chg_current = -(int)results.physical/1000;
		}
	} else {
	#if 0
		if(!chip->chg_present){
				chg_current = 0;
		} 
		else
		{
			if (qpnp_batt_gauge && qpnp_batt_gauge->get_average_current)
			{
				chg_current = qpnp_batt_gauge->get_average_current();
			}
			else {
				pr_err("qpnp-charger no batt gauge assuming 0mA\n");
				chg_current = 0;
			}
		}
	#else
		if (qpnp_batt_gauge && qpnp_batt_gauge->get_average_current)
		{
			chg_current = qpnp_batt_gauge->get_average_current();
		}
		else {
			pr_err("qpnp-charger no batt gauge assuming 0mA\n");
			chg_current = 0;
		}
	#endif
	}
	return chg_current;
}

int opchg_get_prop_batt_health(struct opchg_charger *chip)
{
    union power_supply_propval ret = {0, };
    
	if (chip->battery_missing) {
        ret.intval = POWER_SUPPLY_HEALTH_UNKNOWN;
    }
    else if (chip->batt_hot) {
        ret.intval = POWER_SUPPLY_HEALTH_OVERHEAT;
    }
    else if (chip->batt_cold) {
        ret.intval = POWER_SUPPLY_HEALTH_COLD;
    }
#if 0
	else if (chip->batt_warm) {
        ret.intval = POWER_SUPPLY_HEALTH_WARM;
    }
    else if (chip->batt_cool) {
        ret.intval = POWER_SUPPLY_HEALTH_COOL;
    }
#endif
    else {
        ret.intval = POWER_SUPPLY_HEALTH_GOOD;
    }
    return ret.intval;
}

bool opchg_get_prop_batt_present(struct opchg_charger *chip)
{
	chip->bat_exist = !chip->battery_missing;
	return chip->bat_exist;
}
