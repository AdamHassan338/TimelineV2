#include "clipmodel.h"

ClipModel::ClipModel(int pos, int in, int out, TrackModel* parent) : m_pos(pos),m_in(in),m_out(out), m_parent(parent),m_length(out)
{

}

int ClipModel::pos() const
{
    return m_pos;
}

void ClipModel::setPos(int newPos)
{
    m_pos = newPos;
}

void ClipModel::setParent(TrackModel *newParent)
{
    m_parent = newParent;
}

int ClipModel::in() const
{
    return m_in;
}

void ClipModel::setIn(int newIn)
{
    m_in = newIn;
}

int ClipModel::out() const
{
    return m_out;
}

void ClipModel::setOut(int newOut)
{
    m_out = newOut;
}

int ClipModel::length() const
{
    return m_length;
}
