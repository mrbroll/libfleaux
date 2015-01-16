#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "../headers/editor.hh"
#include "../../../deps/libsdnb/include/sdnb/gap_vector.hh"

using namespace std;
using namespace SDNB;

/* Fleaux::Editor */
Fleaux::Editor::Editor(void)
{
    _data = new GapVector<char>();
    if (_data == NULL) {
        cerr << "ERROR: new failed for SDNB::GapVector<char>" << endl;
        exit(1);
    }

    _cursor = new Cursor(this);
    if (_cursor == NULL) {
        cerr << "ERROR: new failed for Fleaux::Cursor" << endl;
        exit(1);
    }

    size = 0;
}

Fleaux::Editor::~Editor(void)
{
    delete _data;
    delete _cursor;
}

/* Fleaux::Cursor */
void
Fleaux::Cursor::insert(const string& input)
{
    _editor->_data->insert(input.begin(), input.end());
    _index += input.size();
    _editor->size += input.size();
    _setXY();
}

void
Fleaux::Cursor::remove(int length)
{
    length = _editor->_data->remove(length);
    if (length < 0) {
        _index += length;
        _setXY();
    }
    _editor->size -= abs(length);
}

void
Fleaux::Cursor::moveV(int offset)
{
    size_t oldIndex = _index;
    if (offset < 0) {
        offset = max(offset, -(int)_y);
    }
    _y += offset;
    _setIndex();
    _editor->_data->moveGap((int)_index - (int)oldIndex);
}

void
Fleaux::Cursor::moveH(int offset)
{
    size_t oldIndex = _index;
    if (offset < 0) {
        offset = max(offset, -(int)_x);
    } else if (offset > 0) {
        size_t rIndex = _x;
        GapVector<char>::iterator it = _editor->_data->begin() + _index;
        GapVector<char>::iterator end = _editor->_data->end();
        while (it != end && *(it++) != '\n') {
            rIndex++;
        }
        offset = min(offset, (int)rIndex);
    }
    _x += offset;
    _index += offset;
    _editor->_data->moveGap((int)_index - (int)oldIndex);
}

/* Just start at the beginning and count forward.
 * This is a brute force approach, but I just want to 
 * get this working. Once I do, I can come back and
 * optimize these
 */
void
Fleaux::Cursor::_setXY(void)
{
    if (_editor->size > 0) {
        GapVector<char>::iterator it = _editor->_data->begin();
        GapVector<char>::iterator end = it + _index;
        _x = 0;
        _y = 0;
        while (it++ != end) {
            if (*(it - 1) == '\n') {
                _x = 0;
                _y++;
            } else {
                _x++;
            }
        }
    }
}

void
Fleaux::Cursor::_setIndex(void)
{
    if (_editor->size > 0) {
        _index = 0;
        GapVector<char>::iterator it = _editor->_data->begin();
        GapVector<char>::iterator end = _editor->_data->end();
        for (size_t y = 0; y < _y; y++) {
            for (;it != end && *it != '\n'; it++) {
                _index++;
            }
            it++;
            _index++;
        }

        for (size_t x = 0; x < _x && it != end && *(it++) != '\n'; x++) {
            _index++;
        }
    }
}