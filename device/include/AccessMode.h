/*
 * AccessMode.h
 *
 *  Created on: May 11, 2016
 *      Author: Martin Hierholzer
 */

#ifndef MTCA4U_ACCESS_MODE_H
#define MTCA4U_ACCESS_MODE_H

#include <set>

#include "DeviceException.h"

namespace mtca4u {

  /** Enum type with access mode flags for register accessors.
   *
   *  Developers note: when adding new flags, also add the flag in the map of the AccessModeFlags with a string
   *  representation. */
  enum class AccessMode {

    /** Raw access: disable any possible conversion from the original hardware data type into the given UserType.
     *  Obtaining the accessor with a UserType unequal to the actual raw data type will fail and throw a
     *  DeviceException with the id EX_WRONG_PARAMETER.
     *
     *  Note: using this flag will make your code intrinsically dependent on the backend type, since the actual
     *  raw data type must be known. */
    raw,

    /** Make any read blocking until new data has arrived since the last read. This flag may not be suppoerted by
     *  all registers (and backends), in which case a DeviceException with the id NOT_IMPLEMENTED will be thrown. */
    wait_for_new_data

    /* IMPORTANT: When adding flags, don't forget to update AccessModeFlags::getStringMap()! */
  };

  /** Set of AccessMode flags with additional functionality for an easier handling. */
  class AccessModeFlags {

    public:

      /** Constructor initialises from a std::set<AccessMode> */
      AccessModeFlags(const std::set<AccessMode> &flags)
      : _flags(flags)
      {}

      /** Constructor initialises from a brace initialiser list (e.g. "{AccessMode::raw}"). Hint: You can use the
       *  brace initialiser list also without explicitly using the class name, when calling a function which has
       *  an argument of the type AccessModeFlags. */
      AccessModeFlags(const std::initializer_list<AccessMode> &flags)
      : _flags(flags)
      {}

      /** Check if a certain flag is in the set */
      bool has(AccessMode flag) const {
        return ( _flags.count(flag) != 0 );
      }

      /** Check if the set is empty (i.e. no flag has been set) */
      bool empty() const {
        return ( _flags == std::set<AccessMode>() );
      }

      /** Check of any flag which is not in the given set "knownFlags" is set. If an unknown flag has been found, a
       *  DeviceException with the id NOT_IMPLEMENTED is raised. */
      void checkForUnknownFlags(const std::set<AccessMode> &knownFlags) const {
        for(auto flag : _flags) {
          if(knownFlags.count(flag) == 0) {
            throw DeviceException("Access mode flag '"+getString(flag)+"' is not known by this backend.",
                DeviceException::NOT_IMPLEMENTED);
          }
        }
      }

      /** Get a string representation of the given flag */
      const std::string& getString(const AccessMode flag) const {
        return getStringMap().at(flag);
      }

    private:

      /* set of flags */
      std::set<AccessMode> _flags;

      /** return the string map */
      static const std::map<AccessMode, std::string> &getStringMap() {
        static std::map<AccessMode, std::string> m;
        m[AccessMode::raw] = "raw";
        m[AccessMode::wait_for_new_data] = "wait_for_new_data";
        return m;
       }
  };

} /* namespace mtca4u */

#endif /* MTCA4U_ACCESS_MODE_H */
