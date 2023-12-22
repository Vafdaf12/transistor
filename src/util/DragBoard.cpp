#include "DragBoard.h"

void DragBoard::setSelection(const std::vector<Circuit*>& s) { _selection = s; }
void DragBoard::clearSelection() { _selection.clear(); }
const std::vector<Circuit*>& DragBoard::getSelection() const { return _selection; }

void DragBoard::setClipboard(const std::vector<Circuit*>& sel) { _clipboard = sel; }
void DragBoard::clearClipboard() { _clipboard.clear(); }
const std::vector<Circuit*>& DragBoard::getClipboard() const { return _clipboard; }
