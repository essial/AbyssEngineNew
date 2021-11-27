#include "node.h"

void AbyssEngine::Node::UpdateCallback(uint32_t ticks) {
    for (auto &item : Children) {
        if (!item->Active)
            continue;

        UpdateCallback(ticks);
    }
}

void AbyssEngine::Node::RenderCallback(int offsetX, int offsetY) {
    for (auto &item : Children) {
        if (!item->Active || !item->Visible)
            continue;

        RenderCallback(offsetX, offsetY);
    }
}

void AbyssEngine::Node::GetEffectiveLayout(int &x, int &y) {
    x += X;
    y += Y;

    if (Parent != nullptr) {
        Parent->GetEffectiveLayout(x, y);
    }
}

void AbyssEngine::Node::AppendChild(std::unique_ptr<Node> childNode) { Children.push_back(std::move(childNode)); }

void AbyssEngine::Node::RemoveChild(AbyssEngine::Node *nodeRef) {

    int idx = -1;
    for (const auto& node : Children) {
        idx++;

        if (node.get() != nodeRef)
            continue;

        auto it = Children.begin();
        std::advance(it, idx);
        Children.erase(it);

        return;
    }
}
