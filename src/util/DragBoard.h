#pragma once

#include <vector>

#include "circuit/Circuit.h"


class DragBoard {
public:
    void setSelection(const std::vector<Circuit*>&);
    const std::vector<Circuit*>& getSelection() const;

    void clearSelection();

    void setClipboard(const std::vector<Circuit*>&);
    void clearClipboard();
    const std::vector<Circuit*>& getClipboard() const;

private:
    std::vector<Circuit*> _selection;
    std::vector<Circuit*> _clipboard;
};
