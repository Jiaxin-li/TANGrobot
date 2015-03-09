/**
 * OpenDaVINCI - Portable middleware for distributed components.
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "core/base/KeyValueConfiguration.h"

namespace core {
    namespace base {

        KeyValueConfiguration::KeyValueConfiguration() : m_keyValueConfiguration() {}

        KeyValueConfiguration::KeyValueConfiguration(const KeyValueConfiguration &obj) :
                m_keyValueConfiguration(obj.m_keyValueConfiguration) {}

        KeyValueConfiguration& KeyValueConfiguration::operator=(const KeyValueConfiguration &obj) {
            m_keyValueConfiguration = obj.m_keyValueConfiguration;
            return (*this);
        }

        KeyValueConfiguration::~KeyValueConfiguration() {}

        istream& KeyValueConfiguration::operator>>(istream &in) {
            while (!in.eof()) {
                string line;
                getline(in, line);

                // Trying to find comment.
                size_t comment = line.find_first_of("#");

                // Skip lines with preceding #'s.
                if (comment == 1) {
                    continue;
                }

                // Trying to find key-value-pair.
                size_t delimiter = line.find_first_of("=");

                // Skip lines containing no key-value-pairs.
                if (delimiter == string::npos) {
                    continue;
                }

                // Skip lines with incomplete key-value-pairs.
                if (comment < delimiter) {
                    continue;
                }

                // Compute length of value-entry by allowing comments right after values.
                size_t valueLength = line.length();
                if (comment != string::npos) {
                    int32_t realLength = static_cast<int>(comment - (delimiter + 1));
                    valueLength = (realLength < 0) ? 0 : realLength;
                }

                // Skip lines with invalid position pointers.
                if (! ( (delimiter > 0) && (valueLength > 0) ) ) {
                    continue;
                }

                string key = line.substr(0, delimiter);
                string value = line.substr(delimiter + 1, valueLength);

                core::StringToolbox::trim(key);
                core::StringToolbox::trim(value);

                // Skip lines with invalid keys or values.
                if ( (key.length() == 0) ||
                        (value.length() == 0) ) {
                    continue;
                }

                // Transform to lower case for case insensitive searches.
                transform(key.begin(), key.end(), key.begin(), ptr_fun(::tolower));

                m_keyValueConfiguration[key] = value;
            }

            return in;
        }

        KeyValueConfiguration KeyValueConfiguration::getSubsetForSectionRemoveLeadingSectionName(const string &section) const {
            KeyValueConfiguration subset;
            stringstream subsetStringStream;

            // Transform section name to lower case for case insensitive subset building.
            string subsetSection = section;
            transform(subsetSection.begin(), subsetSection.end(), subsetSection.begin(), ptr_fun(::tolower));

            // Print32_t out configuration data.
            map<string, string, wrapper::StringComparator>::const_iterator it = m_keyValueConfiguration.begin();
            for (; it != m_keyValueConfiguration.end(); it++) {
                string key = it->first;
                string::size_type pos = key.find(subsetSection, 0);
                if (pos != string::npos) {
                    subsetStringStream << key.substr(pos + section.length()) << "=" << it->second << endl;
                }
            }

            // Use stringstream to read new subset configuration.
            subsetStringStream >> subset;

            return subset;
        }

        KeyValueConfiguration KeyValueConfiguration::getSubsetForSection(const string &section) const {
            KeyValueConfiguration subset;
            stringstream subsetStringStream;

            // Transform section name to lower case for case insensitive subset building.
            string subsetSection = section;
            transform(subsetSection.begin(), subsetSection.end(), subsetSection.begin(), ptr_fun(::tolower));

            // Print32_t out configuration data.
            map<string, string, wrapper::StringComparator>::const_iterator it = m_keyValueConfiguration.begin();
            for (; it != m_keyValueConfiguration.end(); it++) {
                string key = it->first;
                string::size_type pos = key.find(subsetSection, 0);
                if (pos != string::npos) {
                    subsetStringStream << key << "=" << it->second << endl;
                }
            }

            // Use stringstream to read new subset configuration.
            subsetStringStream >> subset;

            return subset;
        }

        ostream& KeyValueConfiguration::operator<<(ostream &out) const {
            // Print out configuration data.
            map<string, string, wrapper::StringComparator>::const_iterator it = m_keyValueConfiguration.begin();
            for (; it != m_keyValueConfiguration.end(); it++) {
                out << it->first << "=" << it->second << endl;
            }

            return out;
        }

        const vector<string> KeyValueConfiguration::getListOfKeys() const {
            vector<string> listOfKeys;

            map<string, string, wrapper::StringComparator>::const_iterator it = m_keyValueConfiguration.begin();
            for (; it != m_keyValueConfiguration.end(); it++) {
                listOfKeys.push_back(it->first);
            }

            return listOfKeys;
        }

        string KeyValueConfiguration::getValueFor(string key) const {
            string value = "";

            // Transform key name to lower case for case insensitive lookups.
            transform(key.begin(), key.end(), key.begin(), ptr_fun(::tolower));

            // Try to find the key/value.
            map<string, string, wrapper::StringComparator>::const_iterator it = m_keyValueConfiguration.find(key);
            if (it != m_keyValueConfiguration.end()) {
                value = it->second;
            }

            return value;
        }

    }
} // core::base

namespace std {

    ostream& operator<<(ostream& out, const core::base::KeyValueConfiguration &kvc) {
        return kvc.operator << (out);
    }

    istream& operator>>(istream& in, core::base::KeyValueConfiguration &kvc) {
        return kvc.operator >> (in);
    }
}
