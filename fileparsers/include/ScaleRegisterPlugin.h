/*
 * ScaleRegisterPlugin.h
 *
 *  Created on: Feb 25, 2016
 *      Author: Martin Hierholzer
 */

#ifndef MTCA4U_SCALE_REGISTER_PLUGIN_H
#define MTCA4U_SCALE_REGISTER_PLUGIN_H

#include "DataModifierPlugin.h"
#include "NDRegisterAccessor.h"

namespace mtca4u {

  /** RegisterPlugin to scale the register content with a given factor. */
  class ScaleRegisterPlugin : public DataModifierPlugin {

      /** constructor, only internally called from createInstance() */
      ScaleRegisterPlugin(const std::map<std::string, DynamicValue<std::string> > &parameters);
  
    public:

      static boost::shared_ptr<RegisterInfoPlugin> createInstance(const std::map<std::string, DynamicValue<std::string> > &parameters);

    protected:

      template<typename UserType>
      boost::shared_ptr< NDRegisterAccessor<UserType> > decorateRegisterAccessor_impl(
          boost::shared_ptr< NDRegisterAccessor<UserType> > accessor) const;
      DEFINE_VIRTUAL_FUNCTION_TEMPLATE_VTABLE_FILLER(ScaleRegisterPlugin, decorateRegisterAccessor_impl, 1);

      // Helper function to apply scaling factor while copying buffer
      // from underlying accessor to our buffer.
      // Put into separate function to avouid code duplication 
      void applyScalingFactorUnderlyingToThisBuffer();
      
      /** The scaling factor to multiply the data with */
      DynamicValue<double> scalingFactor;
  
  };

} /* namespace mtca4u */

#endif /* MTCA4U_REGISTER_PLUGIN_H */
