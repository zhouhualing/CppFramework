///////////////////////////////////////////////////////////////////////////////
// This file is auto-generated.  Do not modify this file manually. 
// Generated from framework.xml

#include "frameworkConst.h"
#include "StcException.h"

#include <cassert>


#include "MetaClassManager.h"

#include "AttributeTypeBuilder.h"

namespace stc {
namespace framework {


    bool InitAttributeBuilder()
    {
        static bool inited = false;
        if (inited)
        {
            return true;
        }
        inited = true;
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeCommandDescription>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeCommandCategory>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributePropertyDescription>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeHideValue>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeRatePropertyUnits>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeStringPropertyFormat>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeEventMessageAllTypes>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeEventMessageDefaultTypes>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeEventMessagePortDefaultTypes>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeEventMessageProjectDefaultTypes>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeUseClientCommandProcessorOnExecuteInternal>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeIsPropertyResettable>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeSupportsActiveFlag>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeTriggerImmediateBllApply>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeSaveOutputFilesRelativeToTRS>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeUiPropertyName>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeSequencerRuntimeEditable>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeUserDefaultsApplicable>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeUseAutoDeferDelete>());
		stc::framework::AttributeType::AddToFactory(new stc::framework::AttributeBuilderT<AttributeMangleRelativePath>());

        return true;
    }
    static const bool init_ab = InitAttributeBuilder();
}
}
#include "CommandStatusManager.h"
namespace stc {
namespace framework {

static void RegisterCommandStatusMetaInfo()
{
	// no longer needed; will be deprecated
}

}
}

#ifndef WIN32
#include "StcVersion.h"
extern "C" const char* PluginVersion()
{
    return STC_VERSION;
}
#endif

