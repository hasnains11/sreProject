/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef __CHAOSFramework__820DA6A_8242_464B_A93B_22F3F8B08728_EUAbstractApi_h
#define __CHAOSFramework__820DA6A_8242_464B_A93B_22F3F8B08728_EUAbstractApi_h

#include <chaos/common/script/AbstractScriptableClass.h>

namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace script {
                //! forward declaration
                class ScriptableExecutionUnit;
                namespace api {
                    //! class taht permit to interact with execution unit and chaos cript framework
                    class EUDSValueManagement:
                    public chaos::common::script::TemplatedAbstractScriptableClass<EUDSValueManagement> {
                        friend class chaos::cu::control_manager::script::ScriptableExecutionUnit;
                        
                        //execution unit instances
                        ScriptableExecutionUnit *eu_instance;
                    public:
                        ~EUDSValueManagement();
                    protected:
                        EUDSValueManagement(ScriptableExecutionUnit *_eu_instance);
                        
                        
                        //! Add an attribute to the dataset
                        /*!
                         Add an attribute to the execution unit dataset, that represent the thirth type of data managed by EU.
                         The eu dataset have the same meaning of the dataset of the SCCU or RTCU.
                         
                         alias[string] is the name of the dataset attribute
                         description[string] - is the description of the attribute
                         type[number] - the type of the attribute(@chaos::DataType::DataType)
                         direction[number] - is the attribute direction @chaos::DataType::DataSetAttributeIOAttribute
                         */
                        int addDatasetAttribute(const common::script::ScriptInParam& input_parameter,
                                                common::script::ScriptOutParam& output_parameter);
                        
                        //! set the value of an output dataset attribute
                        /*!
                         input parameter
                         attribute_name[string] - the name of the attribute
                         atribute value[as type] - the value of the attribute
                         */
                        int setOutputAttributeValue(const common::script::ScriptInParam& input_parameter,
                                                    common::script::ScriptOutParam& output_parameter);
                        
                        //! return the value of an output dataset attribute
                        /*!
                         input parameter
                         attribute_name[string] - the name of the attribute
                         
                         output attribute
                         atribute value[as type]
                         */
                        int getOutputAttributeValue(const common::script::ScriptInParam& input_parameter,
                                                    common::script::ScriptOutParam& output_parameter);
                        
                        //! return the value of an input dataset attribute
                        /*!
                         input parameter
                         attribute_name[string] - the name of the attribute
                         
                         output attribute
                         atribute value[as type]
                         */
                        int getInputAttributeValue(const common::script::ScriptInParam& input_parameter,
                                                   common::script::ScriptOutParam& output_parameter);
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__820DA6A_8242_464B_A93B_22F3F8B08728_EUAbstractApi_h */
