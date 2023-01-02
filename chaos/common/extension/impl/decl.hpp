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

#ifndef BOOST_EXTENSION_DECL_HPP
#define BOOST_EXTENSION_DECL_HPP

#if defined(__linux__) || defined(__APPLE__)
#  define BOOST_EXTENSION_EXPORT_DECL __attribute__((visibility("default")))
#  define BOOST_EXTENSION_IMPORT_DECL __attribute__((visibility("hidden")))
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(MSC_VER)
#  define BOOST_EXTENSION_EXPORT_DECL __declspec(dllexport)
#  define BOOST_EXTENSION_IMPORT_DECL __declspec(dllimport)
#endif

#endif