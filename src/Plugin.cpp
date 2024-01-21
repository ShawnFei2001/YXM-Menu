#include "Plugin.h"
#include <utility>

#include "ll/api/event/command/ExecuteCommandEvent.h"
#include <ll/api/command/DynamicCommand.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/ListenerBase.h>
#include <ll/api/form/ModalForm.h>
#include <ll/api/form/SimpleForm.h>
#include <ll/api/service/Bedrock.h>
#include <mc/entity/utilities/ActorType.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/actor/player/Player.h>

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) { auto& logger = mSelf.getLogger(); }

bool Plugin::enable() {
    auto& logger = mSelf.getLogger();

    // Register commands
    auto commandRegistry = ll::service::getCommandRegistry();
    if (!commandRegistry) {
        throw std::runtime_error("failed to get command registry");
    }

    auto command = DynamicCommand::createCommand(
        commandRegistry,
        "menu",
        "打开 YXM 菜单 | Open YXM Menu",
        CommandPermissionLevel::Any
    );
    command->setAlias("m");
    command->addOverload();
    command->setCallback(
        [&logger](DynamicCommand const&, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>&) {
            auto entity = origin.getEntity();
            if (entity == nullptr || !entity->isType(ActorType::Player)) {
                output.error("只有玩家可以打开菜单 | Only players can open menu");
                return;
            }
            auto player = (Player*)entity;

            ll::form::SimpleForm form("YXM菜单", "YXM666");
            form.appendButton("Button 1", "", [](Player& pl) {
                pl.sendMessage("Button 1");
            });
            form.sendTo(*player, [](Player& pl, int num) {
                switch (num) {
                case 0:
                    break;
                }
            });
        }
    );
    DynamicCommand::setup(commandRegistry, std::move(command));
    logger.info(R"(
██╗   ██╗██╗  ██╗███╗   ███╗
╚██╗ ██╔╝╚██╗██╔╝████╗ ████║
 ╚████╔╝  ╚███╔╝ ██╔████╔██║
  ╚██╔╝   ██╔██╗ ██║╚██╔╝██║
   ██║   ██╔╝ ██╗██║ ╚═╝ ██║
   ╚═╝   ╚═╝  ╚═╝╚═╝     ╚═╝
                            )");
    logger.info("YXM-Menu Enabled!");
    return true;
}

bool Plugin::disable() {
    auto& logger = mSelf.getLogger();

    // Unregister commands
    auto commandRegistry = ll::service::getCommandRegistry();
    if (!commandRegistry) {
        throw std::runtime_error("failed to get command registry");
    }

    commandRegistry->unregisterCommand("menu");
    logger.info("Disabled");
    return true;
}

} // namespace plugin