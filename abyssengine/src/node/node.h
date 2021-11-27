#ifndef ABYSS_NODE_H
#define ABYSS_NODE_H

#include "../common/events.h"
#include "../engine/engine.h"
#include <vector>

namespace AbyssEngine {

class Node {
  public:
    virtual ~Node() = default;

    virtual void UpdateCallback(Engine &engine, uint32_t ticks);
    virtual void RenderCallback(Engine &engine, int offsetX, int offsetY);
    virtual void MouseEventCallback(Engine &engine, MouseEventInfo eventInfo){};
    void GetEffectiveLayout(int &x, int &y);
    void AppendChild(std::unique_ptr<Node> childNode);
    void RemoveChild(Node *nodeRef);

    int X = 0;
    int Y = 0;
    bool Visible = true;
    bool Active = true;

    Node *Parent;
    std::vector<std::unique_ptr<Node>> Children = std::vector<std::unique_ptr<Node>>();
};

} // namespace AbyssEngine

#endif // ABYSS_NODE_H
