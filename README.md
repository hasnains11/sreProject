# !CHAOS Framework
[0.9.1 Changelog](CHANGELOG.md)

The !CHAOS project is aiming at the development of a new concept of control system and data acquisition framework
providing, with a high level of abstraction, all the services needed for controlling and managing a large scientific,
or non-scientific, infrastructure.  

!CHAOS redefines Control System paradigm by introducing the new concept of Control Service, i.e. a distributed,
scalable provider offering to a general class of clients high-performance services such us data sharing, commands
dispatching, continuous historical data archiving, configuration tools, middle-layer resource management etc.  

Key features and development strategies of !CHAOS are:  

- scalability of performances and size
- integration of all functionalities
- abstraction of services,  devices and data
- easy and modular customization
- extensive data caching for performance boost
- use of high-performance internet software technologies

!CHAOS use the standalone BSON C++ implementation, the source is pulled out from orginal mongodb opensource distribution found on http://www.mongodb.org/ (http://www.10gen.com/)
"BSON" stands for "binary JSON" - a binary storage format that is JSON inspired.
This distribution merely rips it out of the bson repository into its own.

Read the [Documentation](doc/index.md)

For further documentation please visit:

* http://chaos.infn.it                                                     - public information site
* https://chaos-lnf-control.baltig-pages.infn.it/chaosframework-doc        - public user and developer site
* https://chaos-lnf-control.baltig-pages.infn.it/chaosframework            - public source code usefull to collaborate with us

### Compilation report
branch name   |  build status  
--------------|----------------
Master        | [![pipeline status](https://baltig.infn.it/chaos-lnf-control/chaosframework/badges/master/pipeline.svg)](https://baltig.infn.it/chaos-lnf-control/chaosframework/commits/master)
Development   | [![pipeline status](https://baltig.infn.it/chaos-lnf-control/chaosframework/badges/development/pipeline.svg)](https://baltig.infn.it/chaos-lnf-control/chaosframework/commits/development)
Experimental  | [![pipeline status](https://baltig.infn.it/chaos-lnf-control/chaosframework/badges/experimental/pipeline.svg)](https://baltig.infn.it/chaos-lnf-control/chaosframework/commits/experimental)

### Code coverage
[Experimental Branch Covereage Report][4416b59a]
[![coverage report](https://baltig.infn.it/chaos-lnf-control/chaosframework/badges/experimental/coverage.svg)](https://baltig.infn.it/chaos-lnf-control/chaosframework/commits/experimental)

[4416b59a]: https://chaos-lnf-control.baltig-pages.infn.it/chaosframework/ "Experimental Code Coverage"


### keep-a-changelog  (https://getcomposer.org)
keep-a-changelog is a tool for managing the change logs, to install it install composer and then, whitin root chaos directory type: "**_composer install_**"
