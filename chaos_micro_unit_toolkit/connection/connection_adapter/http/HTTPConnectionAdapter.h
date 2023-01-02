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

#ifndef __CHAOSFramework__BDC4C59_4AC1_4BE3_AE95_F497A67E089E_HTTPProtocolAdapter_h
#define __CHAOSFramework__BDC4C59_4AC1_4BE3_AE95_F497A67E089E_HTTPProtocolAdapter_h

#include <chaos_micro_unit_toolkit/connection/connection_adapter/AbstractConnectionAdapter.h>

#include <chaos_micro_unit_toolkit/external_lib/mongoose.h>

namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace connection_adapter {
                namespace http {
                    //! HTTP implementation of connection adapter
                    /*!
                     This adapter realize a standard http websocket connection with the remote endpoint. It
                     uses mongoose library to perform all http operation.
                     */
                    class HTTPConnectionAdapter:
                    public AbstractConnectionAdapter {
                        struct mg_mgr mgr;
                        struct mg_connection *root_conn;
                        static void ev_handler(struct mg_connection *conn,
                                               int event,
                                               void *event_data);
                        //!inherited method
                        int sendRawMessage(chaos::micro_unit_toolkit::data::CDWUniquePtr& message);
                    public:
                        //! point to the protocol constant type define in @chaos::micro_unit_toolkit::connection::ConnectionType
                        static const ConnectionType connection_type;

                        //!defautl contructor
                        /*!
                         \param endpoin has the form 'ws://hostname:port/uri/of/endpoint'
                         \param connection_header continas all header that need to be attacched to websocket http
                         initiation request
                         */
                        HTTPConnectionAdapter(const std::string& endpoin,
                                            const std::string& connection_header);
                        //!default destructor
                        ~HTTPConnectionAdapter();
                        //!inherited method
                        int connect();
                        //!inherited method
                        void poll(int32_t milliseconds_wait = 100);
                        //!inherited method
                        int close();
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__BDC4C59_4AC1_4BE3_AE95_F497A67E089E_HTTPProtocolAdapter_h */
