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

#ifndef __CHAOSFramework__6596C3D_7BB5_4DF5_94F5_70AF5CBF589C_CommandEcho_h
#define __CHAOSFramework__6596C3D_7BB5_4DF5_94F5_70AF5CBF589C_CommandEcho_h

#include "AbstractExternalCommand.h"
namespace chaos {
    namespace agent {
        namespace external_command_pipe {
            
            class CommandEcho:
            public AbstractExternalCommand {
            public:
                CommandEcho(AbstractCommandOutputStream& _cmd_ostream,
                            common::message::MDSMessageChannel& _mds_message_channel);
                ~CommandEcho();
                int execute(ChaosStringVector input_parameter);
            };
        }
    }
}

#endif /* __CHAOSFramework__6596C3D_7BB5_4DF5_94F5_70AF5CBF589C_CommandEcho_h */
