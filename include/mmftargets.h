/**
 * @file mmftargets.h  MMF target configuration options
 *
 * Copyright (C) Metaswitch Networks 2017
 * If license terms are provided to you in a COPYING file in the root directory
 * of the source code repository by which you are accessing this code, then
 * the license outlined in that COPYING file applies to your use.
 * Otherwise no rights are granted except for those provided to you by
 * Metaswitch Networks in a separate written agreement.
 */

#ifndef MMFTARGETS_H__
#define MMFTARGETS_H__

#include <iostream>
#include <memory>
#include "utils.h"
#include "json_parse_utils.h"

/// A representation of an entry in the mmf_targets.json file
class MMFTarget
{
public:
  MMFTarget(const rapidjson::Value& config);

  inline const std::string get_target_name() const {return _name;};
  inline const std::vector<std::string>& get_addresses() const {return _addresses;};

  /// Return whether we should invoke MMF prior to routing a message to any
  /// Application Server associated to this MMFTarget
  inline const bool should_apply_mmf_pre_as() const {return _pre_as;};

  /// Return whether we should invoke MMF after routing a message to any
  /// Application Server associated to this MMFTarget
  inline const bool should_apply_mmf_post_as() const {return _post_as;};

private:
  MMFTarget(const MMFTarget&) = delete;  // Prevent implicit copying

  /// The below methods parse the passed in rapidjson representation of an
  /// MMFTarget, and either set the appropriate member variable or raise an error
  void parse_addresses(const rapidjson::Value& config);
  void parse_name(const rapidjson::Value& config);
  void parse_pre_as(const rapidjson::Value& config);
  void parse_post_as(const rapidjson::Value& config);

  /// The addresses associated with this MMF target, such as DNS A entries
  /// resolving to a cluster of Application Servers
  std::vector<std::string> _addresses;

  /// The name of this MMF target.  The 'mmfcontext' parameter value for any
  /// invocation of MMF is set to this value.
  std::string _name;

  /// Indicates whether we should invoke MMF prior to routing to the
  /// MMF target
  bool _pre_as;

  /// Indicates whether we should invoke MMF after routing to the
  /// MMF target
  bool _post_as;
};

#endif
