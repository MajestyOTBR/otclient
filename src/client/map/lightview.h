/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef LIGHTVIEW_H
#define LIGHTVIEW_H

#include <framework/graphics/framebuffer.h>
#include <framework/graphics/declarations.h>
#include <client/painter/lightviewpainter.h>
#include <client/declarations.h>
#include <client/thing/type/thingtype.h>

struct ShadeBlock {
    int8 floor = -1;
    Point pos;
};

struct LightSource {
    Point pos;
    uint8 color;
    uint16 radius;
    float brightness;
};

class LightView : public LuaObject
{
public:
    LightView(const MapViewPtr& mapView);

    void resize();

    void addLightSource(const Point& mainCenter, const Light& light);

    void setGlobalLight(const Light& light) { m_globalLight = light; m_globalLightColor = Color::from8bit(m_globalLight.color, m_globalLight.intensity / static_cast<float>(UINT8_MAX)); }
    void setFloor(const uint8 floor) { m_currentFloor = floor; }
    void setShade(const Point& point);

    const Light& getGlobalLight() const { return m_globalLight; }

    bool canUpdate() const { return isDark() && m_lightbuffer->canUpdate(); }
    void update() const { if(isDark()) m_lightbuffer->update(); }
    bool isDark() const { return m_globalLight.intensity < 250; }

private:

    Light m_globalLight;
    Color m_globalLightColor;

    FrameBufferPtr m_lightbuffer;
    MapViewPtr m_mapView;

    int8 m_currentFloor;

    std::vector<ShadeBlock> m_shades;
    std::array<std::vector<LightSource>, MAX_Z + 1> m_lights;

    friend class LightViewPainter;
};

#endif
