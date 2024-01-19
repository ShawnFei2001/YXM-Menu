#include "Plugin.h"
#include <utility>

#include "ll/api/event/command/ExecuteCommandEvent.h"
#include <ll/api/command/DynamicCommand.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/ListenerBase.h>
#include <ll/api/form/ModalForm.h>
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
            auto* entity = origin.getEntity();
            if (entity == nullptr || !entity->isType(ActorType::Player)) {
                output.error("只有玩家可以打开菜单 | Only players can open menu");
                return;
            }
            auto                player = (Player*)entity;
            ll::form::ModalForm form(
                "YXM菜单",
                "你喜欢YXM服务器吗?",
                "Yes",
                "No",
                [&logger](Player& player, bool yes) {
                    if (yes) {
                        player.kill();
                        player.sendMessage("我爱'死'YXM了!");
                    } else {
                        player.kill();
                        player.sendMessage("我不喜欢YXM,我不配活在这个世界上...");
                    }
                }
            );

            form.sendTo(*player);
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