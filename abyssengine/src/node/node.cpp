#include "node.h"

void AbyssEngine::Node::UpdateCallback(AbyssEngine::Engine &engine, uint32_t ticks) {
    for (auto &item : Children) {
        if (!item->Active)
            continue;

        UpdateCallback(engine, ticks);
    }
}

void AbyssEngine::Node::RenderCallback(AbyssEngine::Engine &engine, int offsetX, int offsetY) {
    for (auto &item : Children) {
        if (!item->Active || !item->Visible)
            continue;

        RenderCallback(engine, offsetX, offsetY);
    }
}

void AbyssEngine::Node::GetEffectiveLayout(int &x, int &y) {
    x += X;
    y += Y;

    if (Parent != nullptr) {
        Parent->GetEffectiveLayout(x, y);
    }
}

void AbyssEngine::Node::AppendChild(std::unique_ptr<Node> childNode) {
    Children.push_back(std::move(childNode));
}


// --------------------------------------------------------------------------------------------------------------------
