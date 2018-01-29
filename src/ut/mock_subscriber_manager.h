/**
 * @file mock_subscriber_manager.h
 *
 * Copyright (C) Metaswitch Networks 2018
 * If license terms are provided to you in a COPYING file in the root directory
 * of the source code repository by which you are accessing this code, then
 * the license outlined in that COPYING file applies to your use.
 * Otherwise no rights are granted except for those provided to you by
 * Metaswitch Networks in a separate written agreement.
 */

#ifndef MOCK_SUBSCRIBER_MANAGER_H_
#define MOCK_SUBSCRIBER_MANAGER_H_

#include "gmock/gmock.h"
#include "subscriber_manager.h"

class MockSubscriberManager : public SubscriberManager
{
public:
  MockSubscriberManager();
  virtual ~MockSubscriberManager();

  MOCK_METHOD5(register_subscriber, HTTPCode(const std::string& aor_id,
                                             const std::string& server_name,
                                             const Bindings& add_bindings,
                                             Bindings& all_bindings,
                                             SAS::TrailId trail));

  MOCK_METHOD6(reregister_subscriber, HTTPCode(const std::string& aor_id,
                                               const Bindings& updated_bindings,
                                               const std::vector<std::string>& binding_ids_to_remove,
                                               Bindings& all_bindings,
                                               HSSConnection::irs_info& irs_info,
                                               SAS::TrailId trail));

  MOCK_METHOD6(update_bindings, HTTPCode(const HSSConnection::irs_query& irs_query,
                                         const Bindings& updated_bindings,
                                         const std::vector<std::string>& binding_ids_to_remove,
                                         Bindings& all_bindings,
                                         HSSConnection::irs_info& irs_info,
                                         SAS::TrailId trail));

  MOCK_METHOD5(remove_bindings, HTTPCode(const std::string& public_id,
                                         const std::vector<std::string>& binding_ids,
                                         const EventTrigger& event_trigger,
                                         Bindings& bindings,
                                         SAS::TrailId trail));

  MOCK_METHOD4(update_subscription, HTTPCode(const std::string& public_id,
                                             const SubscriptionPair& subscription,
                                             HSSConnection::irs_info& irs_info,
                                             SAS::TrailId trail));

  MOCK_METHOD4(remove_subscription, HTTPCode(const std::string& public_id,
                                             const std::string& subscription_id,
                                             HSSConnection::irs_info& irs_info,
                                             SAS::TrailId trail));

  MOCK_METHOD3(deregister_subscriber, HTTPCode(const std::string& public_id,
                                               const EventTrigger& event_trigger,
                                               SAS::TrailId trail));

  MOCK_METHOD3(get_bindings, HTTPCode(const std::string& public_id,
                                      Bindings& bindings,
                                      SAS::TrailId trail));

  MOCK_METHOD3(get_subscriptions, HTTPCode(const std::string& public_id,
                                           Subscriptions& subscriptions,
                                           SAS::TrailId trail));

  MOCK_METHOD3(get_cached_subscriber_state, HTTPCode(const std::string& public_id,
                                                     HSSConnection::irs_info& irs_info,
                                                     SAS::TrailId trail));

  MOCK_METHOD3(get_subscriber_state, HTTPCode(const HSSConnection::irs_query& irs_query,
                                              HSSConnection::irs_info& irs_info,
                                              SAS::TrailId trail));

  MOCK_METHOD3(update_associated_uris, HTTPCode(const std::string& aor_id,
                                                const AssociatedURIs& associated_uris,
                                                SAS::TrailId trail));

  MOCK_METHOD2(handle_timer_pop, void(const std::string& aor_id,
                                      SAS::TrailId trail));
};

// Custom matcher to see what public identity was on the irs_query that
// functions are called with.
MATCHER_P(IrsQueryWithPublicId, pub_id, "") { return (arg._public_id == pub_id); }

#endif
