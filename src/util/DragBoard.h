#pragma once

#include <vector>

#include "circuit/Circuit.h"


class DragBoard {
public:
    void setSelection(const std::vector<Circuit*>&);
    const std::vector<Circuit*>& getSelection() const;

    void clearSelection();

private:
    std::vector<Circuit*> _selection;
};
