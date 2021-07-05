#pragma once

namespace Lunia {
    namespace XRated {
        namespace Service {

            enum TypeFlags {

                LobbyService = 1 << 1,
                StageService = 1 << 2,
                SquareService = 1 << 3,
                PvpService = 1 << 4,
                ControlService = 1 << 5,
                MonitorService = 1 << 8,
            };

            /* states for extra stage information */
            struct StageStates {
                unsigned NumberOfTeams : 4;  // 0 ~ 15
                unsigned IsPrivate : 1;      // 0 ~ 1

                operator int() const;
                StageStates& operator=(int rhs);
                StageStates();
            };
        }
    }
}
