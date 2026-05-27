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

#include <unistd.h>
#include <netinet/in.h>

// Language: false = EN, true = RU
static bool g_lang_ru = false;
static inline const char* tr(const char* en, const char* ru) {
    return g_lang_ru ? ru : en;
}

class GuiCheats;
class GuiStats;

class GuiMain : public tsl::Gui {
public:
    GuiMain() { }
    ~GuiMain() { }

    virtual tsl::elm::Element* createUI() {
        auto *rootFrame = new tsl::elm::HeaderOverlayFrame();
        rootFrame->setHeader(new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, s32 x, s32 y, s32 w, s32 h) {
            renderer->drawString(APP_TITLE, false, 20, 50, 32, (tsl::defaultOverlayColor));
            renderer->drawString(APP_VERSION, false, 20, 52+23, 15, (tsl::bannerVersionTextColor));

            if (edz::cheat::CheatManager::getProcessID() != 0) {
                renderer->drawString(tr("Program ID", "Программа"), false, 150 +14, 40 -6, 15, (tsl::style::color::ColorText));
                renderer->drawString(tr("Build ID",   "Билд"), false, 150 +14, 60 -6, 15, (tsl::style::color::ColorText));
                renderer->drawString(tr("Process ID", "Процесс"), false, 150 +14, 80 -6, 15, (tsl::style::color::ColorText));
                renderer->drawString(GuiMain::s_runningTitleIDString.c_str(), false, 250 +14, 40 -6, 15, (tsl::style::color::ColorHighlight));
                renderer->drawString(GuiMain::s_runningBuildIDString.c_str(), false, 250 +14, 60 -6, 15, (tsl::style::color::ColorHighlight));
                renderer->drawString(GuiMain::s_runningProcessIDString.c_str(), false, 250 +14, 80 -6, 15, (tsl::style::color::ColorHighlight));
            }
        }));

        auto list = new tsl::elm::List();

        if(edz::cheat::CheatManager::isCheatServiceAvailable()){
            auto cheatsItem = new tsl::elm::ListItem(tr("Cheats", "Читы"));
            cheatsItem->setClickListener([cheatsItem](s64 keys) {
                if (keys & KEY_A) {
                    tsl::changeTo<GuiCheats>("");
                    return true;
                }
                return false;
            });
            list->addItem(cheatsItem);
        } else {
            auto noDmntSvc = new tsl::elm::ListItem(tr("Cheat Service Unavailable!", "Сервис читов недоступен!"));
            list->addItem(noDmntSvc);
        }

        auto statsItem = new tsl::elm::ListItem(tr("System Information", "Системная информация"));
        statsItem->setClickListener([statsItem](s64 keys) {
            if (keys & KEY_A) {
                tsl::changeTo<GuiStats>();
                return true;
            }
            return false;
        });
        list->addItem(statsItem);

        // Language toggle
        auto langItem = new tsl::elm::ListItem(tr("Language", "Язык"));
        langItem->setValue(g_lang_ru ? "RU" : "EN");
        langItem->setClickListener([](s64 keys) -> bool {
            if (keys & KEY_A) {
                g_lang_ru = !g_lang_ru;
                tsl::changeTo<GuiMain>();
                return true;
            }
            return false;
        });
        list->addItem(langItem);

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

        rootFrame->setHeader(new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, s32 x, s32 y, s32 w, s32 h) {
            renderer->drawString(APP_TITLE, false, 20, 50, 32, (tsl::defaultOverlayColor));
            renderer->drawString(tr("Cheats", "Читы"), false, 20, 52+23, 15, (tsl::bannerVersionTextColor));

            if (edz::cheat::CheatManager::getProcessID() != 0) {
                renderer->drawString(tr("Program ID", "Программа"), false, 150 +14, 40 -6, 15, (tsl::style::color::ColorText));
                renderer->drawString(tr("Build ID",   "Билд"), false, 150 +14, 60 -6, 15, (tsl::style::color::ColorText));
                renderer->drawString(tr("Process ID", "Процесс"), false, 150 +14, 80 -6, 15, (tsl::style::color::ColorText));
                renderer->drawString(GuiMain::s_runningTitleIDString.c_str(), false, 250 +14, 40 -6, 15, (tsl::style::color::ColorHighlight));
                renderer->drawString(GuiMain::s_runningBuildIDString.c_str(), false, 250 +14, 60 -6, 15, (tsl::style::color::ColorHighlight));
                renderer->drawString(GuiMain::s_runningProcessIDString.c_str(), false, 250 +14, 80 -6, 15, (tsl::style::color::ColorHighlight));
            }
        }));

        if (edz::cheat::CheatManager::getCheats().size() == 0) {
            auto warning = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h){
                const char* noCheatsTxt = tr("No Cheats loaded!", "Читы не загружены!");
                static const auto iconWidth = renderer->getTextDimensions("", false, 90).first;
                const auto textWidth = renderer->getTextDimensions(noCheatsTxt, false, 25).first;
                renderer->drawString("", false, (tsl::cfg::FramebufferWidth - iconWidth) / 2, 274, 90, (0xFFFF));
                renderer->drawString(noCheatsTxt, false, (tsl::cfg::FramebufferWidth - textWidth) / 2, 360, 25, (0xFFFF));
            });

            rootFrame->setContent(warning);

        } else {
            auto list = new tsl::elm::List();
            std::string head = std::string(tr("Section: ", "Раздел: ")) + this->m_section;

            if(m_section.length() > 0) list->addItem(new tsl::elm::CategoryHeader(head));
            else list->addItem(new tsl::elm::CategoryHeader(tr("Available cheats", "Доступные читы")));

            bool skip = false, inSection = false, submenus = true;
            std::string skipUntil = "";

            for (auto &cheat : edz::cheat::CheatManager::getCheats()) {
                if(cheat->getID() == 1 && cheat->getName().find("--DisableSubmenus--") != std::string::npos)
                    submenus = false;

                if(submenus){
                    if(this->m_section.length() > 0 && !inSection && cheat->getName().find("--SectionStart:" + this->m_section + "--") == std::string::npos) continue;
                    else if(cheat->getName().find("--SectionStart:" + this->m_section + "--") != std::string::npos) { inSection = true; continue; }
                    else if(inSection && cheat->getName().find("--SectionEnd:" + this->m_section + "--") != std::string::npos) break;

                    if(!skip && cheat->getName().find("--SectionStart:") != std::string::npos){
                        std::string name = cheat->getName();
                        replaceAll(name, "--", "");
                        replaceAll(name, "SectionStart:", "");

                        auto cheatsSubmenu = new tsl::elm::ListItem(name);
                        cheatsSubmenu->setClickListener([name = name, cheatsSubmenu](s64 keys) {
                            if (keys & KEY_A) {
                                tsl::changeTo<GuiCheats>(name);
                                return true;
                            }
                            return false;
                        });
                        list->addItem(cheatsSubmenu);
                        this->m_numCheats++;

                        skip = true;
                        skipUntil = "--SectionEnd:" + name + "--";
                    }
                    else if (skip && cheat->getName().compare(skipUntil) == 0){
                        skip = false;
                        skipUntil = "";
                    }
                    else if(!skip && (inSection || this->m_section.length() < 1)) {
                        std::string cheatNameCheck = cheat->getName();
                        replaceAll(cheatNameCheck, ":ENABLED", "");

                        auto cheatToggleItem = new tsl::elm::ToggleListItem(cheatNameCheck, cheat->isEnabled());
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

                    auto cheatToggleItem = new tsl::elm::ToggleListItem(cheatNameCheck, cheat->isEnabled());
                    cheatToggleItem->setStateChangedListener([&cheat](bool state) { cheat->setState(state); });

                    this->m_cheatToggleItems.insert({cheat->getID(), cheatToggleItem});
                    list->addItem(cheatToggleItem);
                    this->m_numCheats++;
                }
            }

            if(this->m_numCheats < 1){
                auto warning = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h){
                    const char* noSubTxt = tr("No Cheats in Submenu!", "В разделе нет читов!");
                    static const auto iconWidth = renderer->getTextDimensions("", false, 90).first;
                    const auto textWidth = renderer->getTextDimensions(noSubTxt, false, 25).first;
                    renderer->drawString("", false, (tsl::cfg::FramebufferWidth - iconWidth) / 2, 250, 90, (0xFFFF));
                    renderer->drawString(noSubTxt, false, (tsl::cfg::FramebufferWidth - textWidth) / 2, 340, 25, (0xFFFF));
                });

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
        auto rootFrame = new tsl::elm::OverlayFrame(APP_TITLE, tr("System Information", "Системная информация"));

        auto infos = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h){

            renderer->drawString(tr("CPU Temperature:", "Темп. CPU:"), false, 63, 200, 18, (tsl::style::color::ColorText));
            renderer->drawString(tr("PCB Temperature:", "Темп. PCB:"), false, 63, 230, 18, (tsl::style::color::ColorText));

            renderer->drawRect(x, 243, w, 1, renderer->a(tsl::style::color::ColorFrame));
            renderer->drawString(tr("CPU Clock:", "Такт. CPU:"), false, 63, 270, 18, (tsl::style::color::ColorText));
            renderer->drawString(tr("GPU Clock:", "Такт. GPU:"), false, 63, 300, 18, (tsl::style::color::ColorText));
            renderer->drawString(tr("MEM Clock:", "Такт. MEM:"), false, 63, 330, 18, (tsl::style::color::ColorText));

            renderer->drawRect(x, 343, w, 1, renderer->a(tsl::style::color::ColorFrame));
            renderer->drawString(tr("Local IP:", "IP-адрес:"), false, 63, 370, 18, (tsl::style::color::ColorText));

            // Temperatures
            static char PCB_temperatureStr[10];
            static char SOC_temperatureStr[10];
            static float tempSOC = 0.0f;
            static float tempPCB = 0.0f;

            ult::ReadSocTemperature(&tempSOC, false);
            ult::ReadPcbTemperature(&tempPCB, false);

            snprintf(SOC_temperatureStr, sizeof(SOC_temperatureStr) - 1, "%.1f \xc2\xb0""C", static_cast<double>(tempSOC));
            snprintf(PCB_temperatureStr, sizeof(PCB_temperatureStr) - 1, "%.1f \xc2\xb0""C", static_cast<double>(tempPCB));

            renderer->drawString(SOC_temperatureStr, false, 258, 200, 18, (tsl::style::color::ColorHighlight));
            renderer->drawString(PCB_temperatureStr, false, 258, 230, 18, (tsl::style::color::ColorHighlight));

            static u32 cpuClock = 0, gpuClock = 0, memClock = 0;

            if (hosversionAtLeast(8,0,0)) {
                clkrstGetClockRate(&this->m_clkrstSessionCpu, &cpuClock);
                clkrstGetClockRate(&this->m_clkrstSessionGpu, &gpuClock);
                clkrstGetClockRate(&this->m_clkrstSessionMem, &memClock);
            } else {
                pcvGetClockRate(PcvModule_CpuBus, &cpuClock);
                pcvGetClockRate(PcvModule_GPU, &gpuClock);
                pcvGetClockRate(PcvModule_EMC, &memClock);
            }

            renderer->drawString(formatString("%.01f MHz", cpuClock / 1'000'000.0F).c_str(), false, 258, 270, 18, (tsl::style::color::ColorHighlight));
            renderer->drawString(formatString("%.01f MHz", gpuClock / 1'000'000.0F).c_str(), false, 258, 300, 18, (tsl::style::color::ColorHighlight));
            renderer->drawString(formatString("%.01f MHz", memClock / 1'000'000.0F).c_str(), false, 258, 330, 18, (tsl::style::color::ColorHighlight));

            if (this->m_ipAddressString == "0.0.0.0")
                renderer->drawString(tr("Offline", "Не в сети"), false, 258, 370, 18, (tsl::style::color::ColorHighlight));
            else
                renderer->drawString(this->m_ipAddressString.c_str(), false, 258, 370, 18, (tsl::style::color::ColorHighlight));

            if(hosversionAtLeast(15,0,0)){
                NifmInternetConnectionType conType;
                u32 wifiStrength;
                NifmInternetConnectionStatus conStatus;
                nifmGetInternetConnectionStatus(&conType, &wifiStrength, &conStatus);
                renderer->drawString(tr("Connection:", "Соединение:"), false, 63, 400, 18, (tsl::style::color::ColorText));
                if(conStatus == NifmInternetConnectionStatus_Connected && conType == NifmInternetConnectionType_WiFi) {
                    const char* strengthStr = tr("(Strong)", "(Отлично)");
                    tsl::Color color = tsl::Color(0x0, 0xF, 0x0, 0xF);
                    if(wifiStrength == 2){
                        strengthStr = tr("(Fair)", "(Хорошо)");
                        color = tsl::Color(0xE, 0xE, 0x2, 0xF);
                    } else if(wifiStrength <= 1){
                        strengthStr = tr("(Poor)", "(Слабый)");
                        color = tsl::Color(0xF, 0x0, 0x0, 0xF);
                    }
                    renderer->drawString("WiFi", false, 258, 400, 18, (tsl::style::color::ColorHighlight));
                    renderer->drawString(strengthStr, false, 303, 400, 18, (color));
                } else if(conStatus == NifmInternetConnectionStatus_Connected && conType == NifmInternetConnectionType_Ethernet){
                    renderer->drawString("Ethernet", false, 258, 400, 18, (tsl::style::color::ColorHighlight));
                } else {
                    renderer->drawString(tr("Disconnected", "Отключено"), false, 258, 400, 18, (tsl::style::color::ColorHighlight));
                }
            } else {
                s32 signalStrength = 0;
                wlaninfGetRSSI(&signalStrength);
                renderer->drawString(tr("WiFi Signal:", "Сигнал WiFi:"), false, 63, 400, 18, (tsl::style::color::ColorText));
                renderer->drawString(formatString("%d dBm", signalStrength).c_str(), false, 258, 400, 18, (tsl::style::color::ColorHighlight));
            }

            // Credits — moved up, Dimasick-git on its own line
            renderer->drawRect(x, 430, w, 1, renderer->a(tsl::style::color::ColorFrame));
            renderer->drawString(tr("Credits:", "Разработчики:"), false, 63, 455, 18, (tsl::style::color::ColorText));
            renderer->drawString("WerWolv, proferabg, ppkantorski", false, 63, 483, 15, (tsl::style::color::ColorHighlight));
            renderer->drawString("& Dimasick-git", false, 63, 508, 15, (tsl::style::color::ColorHighlight));
        });
        rootFrame->setContent(infos);

        return rootFrame;
    }

    virtual void update() { }

private:
    ClkrstSession m_clkrstSessionCpu, m_clkrstSessionGpu, m_clkrstSessionMem;
    u32 m_ipAddress;
    std::string m_ipAddressString;
};




class EdiZonOverlay : public tsl::Overlay {
public:
    EdiZonOverlay() { }
    ~EdiZonOverlay() { }

    void initServices() override {
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
    }

    virtual void exitServices() override {
        if (edz::cheat::CheatManager::isCheatServiceAvailable())
            edz::cheat::CheatManager::exit();
        nifmExit();
        i2cExit();
        wlaninfExit();
        nifmExit();
        clkrstExit();
        pcvExit();
    }

    virtual void onShow() override {
        edz::cheat::CheatManager::reload();
        GuiMain::s_runningTitleIDString   = formatString("%016lX", edz::cheat::CheatManager::getTitleID());
        GuiMain::s_runningBuildIDString   = formatString("%016lX", edz::cheat::CheatManager::getBuildID());
        GuiMain::s_runningProcessIDString = formatString("%lu", edz::cheat::CheatManager::getProcessID());
    }

    std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<GuiMain>();
    }
};


int main(int argc, char **argv) {
    return tsl::loop<EdiZonOverlay>(argc, argv);
}
