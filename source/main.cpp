/**
 * Copyright (C) 2019 - 2020 WerWolv
 *
 * This file is part of EdiZon
 *
 * EdiZon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * EdiZon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EdiZon.  If not, see <http://www.gnu.org/licenses/>.
 */

#define TESLA_INIT_IMPL
#include <exception_wrap.hpp>
#include <tesla.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>

#include <switch/nro.h>
#include <switch/nacp.h>

#include "utils.hpp"
#include "cheat.hpp"
#include "language.hpp"

#include <unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>

static inline const char* tr(const char* en, const char* ru) {
    return edz::language::is_russian() ? ru : en;
}

class GuiCheats;

class GuiStats;

class GuiMain : public tsl::Gui {
public:
    GuiMain() { edz::language::load(); }

    ~GuiMain() { }

    virtual tsl::elm::Element* createUI() {
        auto *rootFrame = new tsl::elm::HeaderOverlayFrame();
        rootFrame->setHeader(new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, s32 x, s32 y, s32 w, s32 h) {
            renderer->drawString(APP_TITLE, false, 20, 50, 32, (tsl::defaultOverlayColor));
            renderer->drawString(APP_VERSION, false, 20, 52+23, 15, (tsl::bannerVersionTextColor));

            if (edz::cheat::CheatManager::getProcessID() != 0) {
                renderer->drawString(tr("Program ID", "ID программы"), false, 150 +14, 40 -6, 15, (tsl::style::color::ColorText));
                renderer->drawString(tr("Build ID", "ID сборки"), false, 150 +14, 60 -6, 15, (tsl::style::color::ColorText));
                renderer->drawString(tr("Process ID", "ID процесса"), false, 150 +14, 80 -6, 15, (tsl::style::color::ColorText));
                renderer->drawString(GuiMain::s_runningTitleIDString.c_str(), false, 250 +14, 40 -6, 15, (tsl::style::color::ColorHighlight));
                renderer->drawString(GuiMain::s_runningBuildIDString.c_str(), false, 250 +14, 60 -6, 15, (tsl::style::color::ColorHighlight));
                renderer->drawString(GuiMain::s_runningProcessIDString.c_str(), false, 250 +14, 80 -6, 15, (tsl::style::color::ColorHighlight));
            }
        }));

        auto list = new tsl::elm::List();

        if(edz::cheat::CheatManager::isCheatServiceAvailable()){
            auto cheatsItem = new tsl::elm::CompactListItem(tr("Cheats", "Читы"));
            cheatsItem->setClickListener([cheatsItem](s64 keys) {
                if (keys & KEY_A) {
                    //tsl::shiftItemFocus(cheatsItem);
                    tsl::changeTo<GuiCheats>("");
                    return true;
                }
                return false;
            });
            list->addItem(cheatsItem);
        } else {
            auto noDmntSvc = new tsl::elm::CompactListItem(tr("Cheat Service Unavailable!", "Сервис читов недоступен!"));
            list->addItem(noDmntSvc);
        }

        auto statsItem  = new tsl::elm::CompactListItem(tr("System Information", "Системная информация"));
        statsItem->setClickListener([statsItem](s64 keys) {
            if (keys & KEY_A) {
                //tsl::shiftItemFocus(statsItem);
                tsl::changeTo<GuiStats>();
                return true;
            }
            return false;
        });
        list->addItem(statsItem);

        auto langItem = new tsl::elm::CompactToggleListItem(tr("Language", "Язык"), edz::language::is_russian());
        langItem->setStateChangedListener([](bool state) {
            edz::language::set_russian(state);
            edz::language::save();
        });
        list->addItem(langItem);

        //list->disableCaching();
        rootFrame->setContent(list);
        return rootFrame;
    }

    virtual void update() { }

public:
    static inline std::string s_runningTitleIDString;
    static inline std::string s_runningProcessIDString;
    static inline std::string s_runningBuildIDString;
    static inline bool b_firstRun = true;
};


class GuiCheats : public tsl::Gui {
public:
    GuiCheats(std::string section) {
        this->m_section = section;
    }
    ~GuiCheats() { }


    virtual tsl::elm::Element* createUI() override {
        auto rootFrame = new tsl::elm::HeaderOverlayFrame(97);

       // bool setOnce = true; // for ensuring header sync with frame caching for header overlayframe

        rootFrame->setHeader(new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, s32 x, s32 y, s32 w, s32 h) {
            renderer->drawString(APP_TITLE, false, 20, 50, 32, (tsl::defaultOverlayColor));
            renderer->drawString(tr("Cheats", "Читы"), false, 20, 52+23, 15, (tsl::bannerVersionTextColor));


            if (edz::cheat::CheatManager::getProcessID() != 0) {
                renderer->drawString(tr("Program ID", "ID программы"), false, 150 +14, 40 -6, 15, (tsl::style::color::ColorText));
                renderer->drawString(tr("Build ID", "ID сборки"), false, 150 +14, 60 -6, 15, (tsl::style::color::ColorText));
                renderer->drawString(tr("Process ID", "ID процесса"), false, 150 +14, 80 -6, 15, (tsl::style::color::ColorText));
                renderer->drawString(GuiMain::s_runningTitleIDString.c_str(), false, 250 +14, 40 -6, 15, (tsl::style::color::ColorHighlight));
                renderer->drawString(GuiMain::s_runningBuildIDString.c_str(), false, 250 +14, 60 -6, 15, (tsl::style::color::ColorHighlight));
                renderer->drawString(GuiMain::s_runningProcessIDString.c_str(), false, 250 +14, 80 -6, 15, (tsl::style::color::ColorHighlight));
            }
        }));

        if (edz::cheat::CheatManager::getCheats().size() == 0) {
            auto warning = new tsl::elm::List();
            warning->addItem(new tsl::elm::CompactDescription(tr("No Cheats loaded!", "Читы не загружены!")));

            rootFrame->setContent(warning);

        } else {
            auto list = new tsl::elm::List();
            std::string head = tr("Section: ", "Раздел: ") + this->m_section;

            if(m_section.length() > 0) list->addItem(new tsl::elm::CompactCategoryHeader(head));
            else list->addItem(new tsl::elm::CompactCategoryHeader(tr("Available cheats", "Доступные читы")));

            bool skip = false, inSection = false, submenus = true;
            std::string skipUntil = "";

            for (auto &cheat : edz::cheat::CheatManager::getCheats()) {
                if(cheat->getID() == 1 && cheat->getName().find("--DisableSubmenus--") != std::string::npos)
                    submenus = false;

                if(submenus){
                    // Find section start and end
                    if(this->m_section.length() > 0 && !inSection && cheat->getName().find("--SectionStart:" + this->m_section + "--") == std::string::npos) continue;
                    else if(cheat->getName().find("--SectionStart:" + this->m_section + "--") != std::string::npos) { inSection = true; continue; }
                    else if(inSection && cheat->getName().find("--SectionEnd:" + this->m_section + "--") != std::string::npos) break;

                    // new section
                    if(!skip && cheat->getName().find("--SectionStart:") != std::string::npos){

                        //remove formatting
                        std::string name = cheat->getName();
                        replaceAll(name, "--", "");
                        replaceAll(name, "SectionStart:", "");

                        //create submenu button
                        auto cheatsSubmenu = new tsl::elm::CompactListItem(name);
                        cheatsSubmenu->setClickListener([name = name, cheatsSubmenu](s64 keys) {
                            if (keys & KEY_A) {
                                //tsl::shiftItemFocus(cheatsSubmenu);
                                tsl::changeTo<GuiCheats>(name);
                                return true;
                            }
                            return false;
                        });
                        list->addItem(cheatsSubmenu);
                        this->m_numCheats++;

                        //skip over items in section
                        skip = true;
                        skipUntil = "--SectionEnd:" + name + "--";
                    }
                    // found end of child section
                    else if (skip && cheat->getName().compare(skipUntil) == 0){
                        skip = false;
                        skipUntil = "";
                    }
                    // items to add to section
                    else if(!skip && (inSection || this->m_section.length() < 1)) {
                        std::string cheatNameCheck = cheat->getName();
                        replaceAll(cheatNameCheck, ":ENABLED", "");

                        auto cheatToggleItem = new tsl::elm::CompactToggleListItem(/*formatString("%d:%s: %s", cheat->getID(), (cheat->isEnabled() ? "y" : "n"),*/ cheatNameCheck/*.c_str()).c_str()*/, cheat->isEnabled());
                        cheatToggleItem->setStateChangedListener([&cheat](bool state) { cheat->setState(state);});

                        this->m_cheatToggleItems.insert({cheat->getID(), cheatToggleItem});
                        list->addItem(cheatToggleItem);
                        this->m_numCheats++;
                    }
                } else {
                    if(cheat->getName().find("--SectionStart:") != std::string::npos || cheat->getName().find("--SectionEnd:") != std::string::npos || cheat->getName().find("--DisableSubmenus--") != std::string::npos)
                        continue;

                    std::string cheatNameCheck = cheat->getName();
                    replaceAll(cheatNameCheck, ":ENABLED", "");

                    auto cheatToggleItem = new tsl::elm::CompactToggleListItem(cheatNameCheck, cheat->isEnabled());
                    cheatToggleItem->setStateChangedListener([&cheat](bool state) { cheat->setState(state); });

                    this->m_cheatToggleItems.insert({cheat->getID(), cheatToggleItem});
                    list->addItem(cheatToggleItem);
                    this->m_numCheats++;
                }
            }

            //list->disableCaching();

            // display if no cheats in submenu
            if(this->m_numCheats < 1){
                auto warning = new tsl::elm::List();
                warning->addItem(new tsl::elm::CompactDescription(tr("No Cheats in Submenu!", "В подменю нет читов!")));
                rootFrame->setContent(warning);
            } else rootFrame->setContent(list);
        }

        return rootFrame;
    }

    void replaceAll(std::string& str, const std::string& from, const std::string& to) {
        if(from.empty())
            return;
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    virtual void update() override {
        for (auto const& [cheatId, toggleElem] : this->m_cheatToggleItems)
            for(auto &cheat : edz::cheat::CheatManager::getCheats())
                if(cheat->getID() == cheatId)
                    toggleElem->setState(cheat->isEnabled());
    }

private:
    int m_numCheats = 0;
    std::string m_section;
    std::map<u32, tsl::elm::ToggleListItem*> m_cheatToggleItems;
};

class GuiStats : public tsl::Gui {
public:
    GuiStats() {
        if (hosversionAtLeast(8,0,0)) {
            clkrstOpenSession(&this->m_clkrstSessionCpu, PcvModuleId_CpuBus, 3);
            clkrstOpenSession(&this->m_clkrstSessionGpu, PcvModuleId_GPU, 3);
            clkrstOpenSession(&this->m_clkrstSessionMem, PcvModuleId_EMC, 3);
        }

        tsl::hlp::doWithSmSession([this]{
            nifmGetCurrentIpAddress(&this->m_ipAddress);
            this->m_ipAddressString = formatString("%d.%d.%d.%d", this->m_ipAddress & 0xFF, (this->m_ipAddress >> 8) & 0xFF, (this->m_ipAddress >> 16) & 0xFF, (this->m_ipAddress >> 24) & 0xFF);
        });

    }
    ~GuiStats() {
        if (hosversionAtLeast(8,0,0)) {
            clkrstCloseSession(&this->m_clkrstSessionCpu);
            clkrstCloseSession(&this->m_clkrstSessionGpu);
            clkrstCloseSession(&this->m_clkrstSessionMem);
        }
     }

    virtual tsl::elm::Element* createUI() override {
        auto* frame = new tsl::elm::OverlayFrame(APP_TITLE, tr("System Information", "Системная информация"));
        auto* list = new tsl::elm::List();
        m_info[0] = new tsl::elm::CompactListItem(tr("CPU Temperature:", "Температура CPU:"), "--");
        list->addItem(m_info[0]);
        m_info[1] = new tsl::elm::CompactListItem(tr("PCB Temperature:", "Температура PCB:"), "--");
        list->addItem(m_info[1]);
        m_info[2] = new tsl::elm::CompactListItem(tr("CPU Clock:", "Частота CPU:"), "--");
        list->addItem(m_info[2]);
        m_info[3] = new tsl::elm::CompactListItem(tr("GPU Clock:", "Частота GPU:"), "--");
        list->addItem(m_info[3]);
        m_info[4] = new tsl::elm::CompactListItem(tr("MEM Clock:", "Частота MEM:"), "--");
        list->addItem(m_info[4]);
        m_info[5] = new tsl::elm::CompactListItem(tr("Local IP:", "Локальный IP:"), "--");
        list->addItem(m_info[5]);
        m_info[6] = new tsl::elm::CompactListItem(tr("Connection:", "Подключение:"), "--");
        list->addItem(m_info[6]);
        list->addItem(new tsl::elm::CompactCategoryHeader(tr("Credits:", "Разработчики:")));
        list->addItem(new tsl::elm::CompactDescription("WerWolv, proferabg, ppkantorski & Dimasick-git"));
        frame->setContent(list);
        update();
        return frame;
    }

    virtual void update() override {
        if (!m_info[0]) return;
        float soc = 0.0f, pcb = 0.0f;
        ult::ReadSocTemperature(&soc, false);
        ult::ReadPcbTemperature(&pcb, false);
        m_info[0]->setValue(formatString("%.1f \u00b0C", static_cast<double>(soc)));
        m_info[1]->setValue(formatString("%.1f \u00b0C", static_cast<double>(pcb)));
        static u32 cpu = 0, gpu = 0, mem = 0;
        if (hosversionAtLeast(8, 0, 0)) {
            clkrstGetClockRate(&m_clkrstSessionCpu, &cpu);
            clkrstGetClockRate(&m_clkrstSessionGpu, &gpu);
            clkrstGetClockRate(&m_clkrstSessionMem, &mem);
        } else {
            pcvGetClockRate(PcvModule_CpuBus, &cpu);
            pcvGetClockRate(PcvModule_GPU, &gpu);
            pcvGetClockRate(PcvModule_EMC, &mem);
        }
        m_info[2]->setValue(formatString("%.1f MHz", cpu / 1'000'000.0F));
        m_info[3]->setValue(formatString("%.1f MHz", gpu / 1'000'000.0F));
        m_info[4]->setValue(formatString("%.1f MHz", mem / 1'000'000.0F));
        if (R_SUCCEEDED(nifmGetCurrentIpAddress(&m_ipAddress))) {
            m_ipAddressString = formatString("%u.%u.%u.%u", m_ipAddress & 0xFF, (m_ipAddress >> 8) & 0xFF, (m_ipAddress >> 16) & 0xFF, (m_ipAddress >> 24) & 0xFF);
        }
        m_info[5]->setValue(m_ipAddressString == "0.0.0.0" ? tr("Offline", "Не в сети") : m_ipAddressString);
        if (hosversionAtLeast(15, 0, 0)) {
            NifmInternetConnectionType type = {};
            u32 strength = 0;
            NifmInternetConnectionStatus status = {};
            Result rc = nifmGetInternetConnectionStatus(&type, &strength, &status);
            if (R_SUCCEEDED(rc) && status == NifmInternetConnectionStatus_Connected) {
                if (type == NifmInternetConnectionType_WiFi) {
                    const char* quality = strength > 2 ? tr("(Strong)", "(Отлично)") : strength == 2 ? tr("(Fair)", "(Хорошо)") : tr("(Poor)", "(Слабо)");
                    m_info[6]->setValue(std::string("WiFi ") + quality);
                } else {
                    m_info[6]->setValue("Ethernet");
                }
            } else {
                m_info[6]->setValue(tr("Disconnected", "Отключено"));
            }
        } else {
            s32 signal = 0;
            m_info[6]->setValue(R_SUCCEEDED(wlaninfGetRSSI(&signal)) ? formatString("%d dBm", signal) : "--");
        }
    }

private:
    tsl::elm::CompactListItem* m_info[7] = {};
    ClkrstSession m_clkrstSessionCpu = {}, m_clkrstSessionGpu = {}, m_clkrstSessionMem = {};
    u32 m_ipAddress = 0;
    std::string m_ipAddressString = "0.0.0.0";
};




class EdiZonOverlay : public tsl::Overlay {
public:
    EdiZonOverlay() { }
    ~EdiZonOverlay() { }

    void initServices() override {
        // GDB Check & Saved Cheat Enabling
        if(edz::cheat::CheatManager::isCheatServiceAvailable()){
            edz::cheat::CheatManager::initialize();
            for (auto &cheat : edz::cheat::CheatManager::getCheats()) {
                if(cheat->getName().find(":ENABLED") != std::string::npos){
                    cheat->setState(true);
                }
            }
        }
        clkrstInitialize();
        pcvInitialize();

        i2cInitialize();
        nifmInitialize(NifmServiceType_User);
        if (hosversionBefore(15, 0, 0)) wlaninfInitialize();
    }

    virtual void exitServices() override {
        if (edz::cheat::CheatManager::isCheatServiceAvailable())
            edz::cheat::CheatManager::exit();
        nifmExit();
        i2cExit();
        if (hosversionBefore(15, 0, 0)) wlaninfExit();
        clkrstExit();
        pcvExit();

    }

    virtual void onShow() override {
        edz::cheat::CheatManager::reload();
        GuiMain::s_runningTitleIDString     = formatString("%016lX", edz::cheat::CheatManager::getTitleID());
        GuiMain::s_runningBuildIDString     = formatString("%016lX", edz::cheat::CheatManager::getBuildID());
        GuiMain::s_runningProcessIDString   = formatString("%lu", edz::cheat::CheatManager::getProcessID());
    }

    std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<GuiMain>();
    }


};


int main(int argc, char **argv) {
    return tsl::loop<EdiZonOverlay>(argc, argv);
}
