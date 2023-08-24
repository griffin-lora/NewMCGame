#include "WorldInfoSign.hpp"

IPlugin* create() {
    return new WorldInfoSign();
}

void destroy(IPlugin* plugin) {
    delete plugin;
}

void WorldInfoSign::setup(Game& game) {
    auto& billboardManager = game.getBillboardManager();

    billboardManager.addBillboard(glm::vec3(0, 50, 0), "<placeholder>", "worldInfoSign");
    billboard = billboardManager.getBillboardByID("worldInfoSign");

    billboard->setText("World info sign");
    billboard->setScale(glm::vec2(5));
}

void WorldInfoSign::frameUpdate(Game& game) {
    static int i = 0;
    if (i % 100 == 0) {
        std::string billboardText = "# Polygons: ";
        billboardText += std::to_string(game.getWorld().getNVertices() / 3);
        billboardText += " # Chunks: ";
        billboardText += std::to_string(game.getWorld().getChunks().size());

        billboard->setText(billboardText);
    }

    i++;
}

void WorldInfoSign::cleanup(Game& game) {

}

std::string WorldInfoSign::getPluginName() {
    return "WorldInfoSign";
}

IPlugin::Version WorldInfoSign::getPluginVersion() {
    return Version { 1, 0 };
}