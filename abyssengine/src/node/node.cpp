#include "node.h"

void AbyssEngine::Node::UpdateCallback(uint32_t ticks) {
    for (auto &item : Children) {
        if (!item->Active)
            continue;

        item->UpdateCallback(ticks);
    }
}

void AbyssEngine::Node::RenderCallback(int offsetX, int offsetY) {
    for (auto &item : Children) {
        if (!item->Active || !item->Visible)
            continue;

        item->RenderCallback(X + offsetX, Y + offsetY);
    }
}

void AbyssEngine::Node::GetEffectiveLayout(int &x, int &y) {
    x += X;
    y += Y;

    if (Parent != nullptr) {
        Parent->GetEffectiveLayout(x, y);
    }
}

void AbyssEngine::Node::AppendChild(Node *childNode) {
    if (childNode == nullptr)
        throw std::runtime_error("Attempted to append null node!");

    Children.push_back(childNode);
}

void AbyssEngine::Node::RemoveChild(AbyssEngine::Node *nodeRef) {

    int idx = -1;
    for (const auto &node : Children) {
        idx++;

        if (node != nodeRef)
            continue;

        auto it = Children.begin();
        std::advance(it, idx);
        Children.erase(it);

        return;
    }
}
void AbyssEngine::Node::RemoveAllChildren() { Children.clear(); }
void AbyssEngine::Node::SetPosition(int x, int y) {
    X = x;
    Y = y;
}
std::tuple<int, int> AbyssEngine::Node::GetPosition() { return {X, Y}; }
void AbyssEngine::Node::SetActive(bool active) { Active = active; }
bool AbyssEngine::Node::GetActive() const { return Active; }
void AbyssEngine::Node::SetVisible(bool visible) { Visible = visible; }
bool AbyssEngine::Node::GetVisible() const { return Visible; }
