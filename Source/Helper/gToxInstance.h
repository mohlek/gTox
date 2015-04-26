/**
    gTox a GTK-based tox-client - https://github.com/KoKuToru/gTox.git

    Copyright (C) 2015  Luca Béla Palkovics
    Copyright (C) 2015  Maurice Mohlek

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
**/
#ifndef H_GTOX_INSTANCE
#define H_GTOX_INSTANCE
#include <Tox/Toxmm.h>

class gToxChild;
class gToxInstance {
        friend class gToxChild;
    public:
        typedef std::function<void(const ToxEvent&)> EFunc;
        class CallbackHandler {
            private:
                struct CallbackHandlerPrivate {
                        gToxInstance* m_parent;
                        EFunc m_func;
                };

                std::shared_ptr<CallbackHandlerPrivate> m_mem;

            public:
                CallbackHandler() {}
                CallbackHandler(gToxInstance* parent, EFunc func) {
                    m_mem = std::shared_ptr<CallbackHandlerPrivate>(new CallbackHandlerPrivate{parent, func},
                        //DELETER FUNCTION:
                        [](CallbackHandlerPrivate* m) {
                            m->m_parent->uninstall_observer(&(m->m_func));
                            delete m;
                        });

                    m_mem->m_parent->install_observer(&(m_mem->m_func));
                }

                void operator()(const ToxEvent& e) {
                    if (m_mem) {
                        m_mem->m_func(e);
                    }
                }

                void reset() {
                    m_mem.reset();
                }
        };

        Toxmm& tox();

        CallbackHandler add_observer(EFunc callback);
        /**
         * @brief Call all installed callbacks with the event data
         * @param data
         */
        void notify_observer(const ToxEvent& data);

    private:
        Toxmm m_tox;
        std::vector<EFunc*> m_callback_list;

    protected:
        void install_observer(EFunc* func);
        void uninstall_observer(EFunc* func);
};
#endif