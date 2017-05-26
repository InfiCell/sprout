/**
 * @file registration_utils.h
 *
 * Copyright (C) Metaswitch Networks 2017
 * If license terms are provided to you in a COPYING file in the root directory
 * of the source code repository by which you are accessing this code, then
 * the license outlined in that COPYING file applies to your use.
 * Otherwise no rights are granted except for those provided to you by
 * Metaswitch Networks in a separate written agreement.
 */

#ifndef REGISTRATION_UTILS_H__
#define REGISTRATION_UTILS_H__

extern "C" {
#include <pjsip.h>
#include <pjlib-util.h>
#include <pjlib.h>
#include <stdint.h>
}

#include <string>
#include "subscriber_data_manager.h"
#include "ifchandler.h"
#include "hssconnection.h"
#include "snmp_success_fail_count_table.h"

namespace RegistrationUtils {

void init(SNMP::RegistrationStatsTables* third_party_reg_stats_tables_arg,
          bool force_third_party_register_body_arg);

bool remove_bindings(SubscriberDataManager* sdm,
                     std::vector<SubscriberDataManager*> remote_sdms,
                     HSSConnection* hss,
                     const std::string& aor,
                     const std::string& binding_id,
                     const std::string& dereg_type,
                     SAS::TrailId trail,
                     HTTPCode* hss_status_code = nullptr);

void register_with_application_servers(Ifcs& ifcs,
                                       SubscriberDataManager* sdm,
                                       std::vector<SubscriberDataManager*> remote_sdms,
                                       HSSConnection* hss,
                                       pjsip_msg* received_register_msg,
                                       pjsip_msg* ok_response_msg,
                                       int expires,
                                       bool is_initial_registration,
                                       const std::string& served_user,
                                       SAS::TrailId trail);

void deregister_with_application_servers(Ifcs&,
                                         SubscriberDataManager* sdm,
                                         std::vector<SubscriberDataManager*> remote_sdms,
                                         HSSConnection* hss,
                                         const std::string&,
                                         SAS::TrailId trail);
}

#endif
