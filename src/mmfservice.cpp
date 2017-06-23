/**
 * @file mmfservice.cpp The MMF Config handler.
 *
 * Copyright (C) Metaswitch Networks 2017
 * If license terms are provided to you in a COPYING file in the root directory
 * of the source code repository by which you are accessing this code, then
 * the license outlined in that COPYING file applies to your use.
 * Otherwise no rights are granted except for those provided to you by
 * Metaswitch Networks in a separate written agreement.
 */

#include <sys/stat.h>
#include <fstream>

#include "mmfservice.h"
#include "sprout_pd_definitions.h"
#include "utils.h"
#include "rapidjson/error/en.h"
#include "json_parse_utils.h"

MMFService::MMFService(Alarm* alarm,
                       std::string configuration):
  _alarm(alarm),
  _configuration(configuration),
  _updater(NULL),
  _mmf_config(std::shared_ptr<MMFService::MMFMap>(new MMFService::MMFMap))
{
  // Create an updater to keep the invoking of MMF configured correctly.
  _updater = new Updater<void, MMFService>
                              (this, std::mem_fun(&MMFService::update_config));
}

MMFService::~MMFService()
{
  delete _updater; _updater = nullptr;
  delete _alarm; _alarm = nullptr;
}

void MMFService::update_config()
{
  // Check whether the file exists.
  struct stat s;
  rapidjson::Document doc;

  TRC_DEBUG("stat (%s) returns %d", _configuration.c_str(),
            stat(_configuration.c_str(), &s));
  if ((stat(_configuration.c_str(), &s) != 0) &&
      (errno == ENOENT))
  {
    TRC_STATUS("No MMF configuration found (file %s does not exist)",
               _configuration.c_str());
    CL_SPROUT_MMF_FILE_MISSING.log();
    set_alarm();
    return;
  }

  TRC_STATUS("Loading MMF configuration from %s", _configuration.c_str());

  // Check whether the file is empty.
  std::ifstream fs(_configuration.c_str());
  std::string mmf_str((std::istreambuf_iterator<char>(fs)),
                        std::istreambuf_iterator<char>());
  if (mmf_str == "")
  {
    TRC_ERROR("Failed to read MMF configuration data from %s",
              _configuration.c_str());
    CL_SPROUT_MMF_FILE_EMPTY.log();
    set_alarm();
    return;
  }

  TRC_DEBUG("Read MMF config file from stream successfully.");

  // Check the file contains valid JSON
  try
  {
    doc.Parse<0>(mmf_str.c_str());
    TRC_DEBUG("Parsed into JSON Doc.");

    if (doc.HasParseError())
    {
      TRC_ERROR("Failed to read the MMF configuration data from %s "
                "due to a JSON parse error.", _configuration.c_str());
      TRC_DEBUG("Badly formed configuration data: %s", mmf_str.c_str());
      TRC_ERROR("Error: %s", rapidjson::GetParseError_En(doc.GetParseError()));
      JSON_FORMAT_ERROR();
    }

    std::shared_ptr<MMFService::MMFMap> mmf_config(nullptr);

    // This throws a JsonFormatError if the MMF configuration data is invalid
    mmf_config = read_config(doc);

    // Now that we have the mmf config lock, free the memory from the old
    // mmf config objects, and start pointing at the new ones.
    TRC_DEBUG("Delete old MMF config.");
    _mmf_config = mmf_config;

    clear_alarm();
    TRC_DEBUG("Updated MMF config.");
  }
  catch (JsonFormatError err)
  {
    TRC_ERROR("Badly formed MMF targets configuration file. If good MMF targets "
              "config was previously loaded, the S-CSCF will continue to use it.");
    CL_SPROUT_MMF_FILE_INVALID.log();
    set_alarm();
  }
}

std::shared_ptr<MMFService::MMFMap> MMFService::read_config(rapidjson::Document& doc)
{
  std::shared_ptr<MMFService::MMFMap> mmf_config(new MMFService::MMFMap);

  TRC_DEBUG("Reading MMF Config");

  if (!doc.HasMember("mmf_targets"))
  {
    TRC_STATUS("No MMF config present in the %s file.  Sprout will not apply "
               "MMF to any calls.", _configuration.c_str());
  }
  else
  {
    const rapidjson::Value& mmf_targets = doc["mmf_targets"];

    // Iterate over MMF targets in the config file
    for (rapidjson::Value::ConstValueIterator mmf_it = mmf_targets.Begin();
         mmf_it != mmf_targets.End();
         ++mmf_it)
    {
      // Throws a JsonFormatError if the target is invalid
      MMFTargetPtr target(new MMFTarget(*mmf_it));

      for (std::string address : target->get_addresses())
      {
        if (mmf_config->find(address) != mmf_config->end())
        {
          TRC_ERROR("Duplicate config present in the %s configuration file for"
                    "the address: '%s'", _configuration.c_str(), address.c_str());
          JSON_FORMAT_ERROR();
        }

        mmf_config->insert(std::make_pair(address, target));
      }
    }
  }

  return mmf_config;
}

MMFService::MMFTargetPtr MMFService::get_config_for_server(std::string server_domain)
{
  std::shared_ptr<MMFService::MMFMap> mmf_config = _mmf_config;

  if (mmf_config->find(server_domain) != mmf_config->end())
  {
    return mmf_config->at(server_domain);
  }
  else
  {
    return nullptr;
  }
}

void MMFService::set_alarm()
{
  if (_alarm)
  {
    _alarm->set();
  }
}

void MMFService::clear_alarm()
{
  if (_alarm)
  {
    _alarm->clear();
  }
}
