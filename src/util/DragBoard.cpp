#include "DragBoard.h"


    void DragBoard::setSelection(const std::vector<Circuit*>& s) { _selection = s;}
    const std::vector<Circuit*>& DragBoard::getSelection() const { return _selection; }

    void DragBoard::clearSelection() {
        _selection.clear();
    }
