#ifndef ABYSS_NODE_H
#define ABYSS_NODE_H

#include "../common/events.h"
#include <vector>
#include <memory>

namespace AbyssEngine {

class Node {
  public:
    virtual ~Node() = default;

    virtual void UpdateCallback(uint32_t ticks);
    virtual void RenderCallback(int offsetX, int offsetY);
    virtual void MouseEventCallback(MouseEventInfo eventInfo){};
    void GetEffectiveLayout(int &x, int &y);
    void AppendChild(std::unique_ptr<Node> childNode);
    void RemoveChild(Node *nodeRef);

    int X = 0;
    int Y = 0;
    bool Visible = true;
    bool Active = true;

    Node *Parent = nullptr;
    std::vector<std::unique_ptr<Node>> Children = std::vector<std::unique_ptr<Node>>();
};

} // namespace AbyssEngine

#endif // ABYSS_NODE_H
