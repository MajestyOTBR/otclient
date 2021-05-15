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

#ifndef TILE_H
#define TILE_H

#include <framework/luaengine/luaobject.h>
#include <client/thing/creature/creature.h>
#include <client/declarations.h>
#include <client/thing/effect.h>
#include <client/thing/item.h>
#include <client/map/mapview.h>
#include <client/painter/tilepainter.h>

enum tileflags_t : uint32
{
    TILESTATE_NONE = 0,
    TILESTATE_PROTECTIONZONE = 1 << 0,
    TILESTATE_TRASHED = 1 << 1,
    TILESTATE_OPTIONALZONE = 1 << 2,
    TILESTATE_NOLOGOUT = 1 << 3,
    TILESTATE_HARDCOREZONE = 1 << 4,
    TILESTATE_REFRESH = 1 << 5,

    // internal usage
    TILESTATE_HOUSE = 1 << 6,
    TILESTATE_TELEPORT = 1 << 17,
    TILESTATE_MAGICFIELD = 1 << 18,
    TILESTATE_MAILBOX = 1 << 19,
    TILESTATE_TRASHHOLDER = 1 << 20,
    TILESTATE_BED = 1 << 21,
    TILESTATE_DEPOT = 1 << 22,

    TILESTATE_LAST = 1 << 23
};

class Tile : public LuaObject
{
public:
    enum {
        MAX_THINGS = 10
    };

    Tile(const Position& position);

    void onAddVisibleTileList(const MapViewPtr& mapView);
    void addWalkingCreature(const CreaturePtr& creature);
    void removeWalkingCreature(const CreaturePtr& creature);

    void addThing(const ThingPtr& thing, int stackPos);
    bool removeThing(const ThingPtr thing);

    EffectPtr getEffect(uint16 id);
    ThingPtr getThing(int stackPos);
    ThingPtr getTopThing();
    int8 getThingStackPos(const ThingPtr& thing);

    ThingPtr getTopLookThing();
    ThingPtr getTopUseThing();
    ThingPtr getTopMoveThing();
    ThingPtr getTopMultiUseThing();
    CreaturePtr getTopCreature(bool checkAround = false);

    uint8 getDrawElevation() { return m_drawElevation; }

    const Position& getPosition() { return m_position; }
    const std::vector<ThingPtr>& getThings() { return m_things; }
    const std::vector<CreaturePtr> getCreatures();
    const std::vector<CreaturePtr>& getWalkingCreatures() { return m_walkingCreatures; }

    const std::array<Position, 8> getPositionsAround() { return m_positionsAround; }

    ItemPtr getGround();
    uint8 getThingCount() { return m_things.size() + m_effects.size(); }
    uint16 getGroundSpeed();
    uint8 getMinimapColorByte();
    std::vector<ItemPtr> getItems();

    void clean() { m_things.clear(); }
    void updateFlag(const ThingPtr& thing, bool add);
    void overwriteMinimapColor(uint8 color) { m_minimapColor = color; }

    uint32 getFlags() { return m_flags; }
    void remFlag(uint32 flag) { m_flags &= ~flag; }
    void setFlag(uint32 flag) { m_flags |= flag; }
    void setFlags(uint32 flags) { m_flags = flags; }
    bool hasFlag(uint32 flag) { return (m_flags & flag) == flag; }

    void setHouseId(uint32 hid) { m_houseId = hid; }
    uint32 getHouseId() { return m_houseId; }

    void select();
    void unselect();
    bool isSelected() { return m_highlight.enabled; }

    uint8 getElevation() const { return m_countFlag.elevation; }

    bool limitsFloorsView(bool isFreeView = false);

    bool canErase() { return m_walkingCreatures.empty() && m_effects.empty() && isEmpty() && m_flags == 0 && m_minimapColor == 0; }
    bool mustHookEast() { return m_countFlag.hasHookEast; }
    bool mustHookSouth() { return m_countFlag.hasHookSouth; }

    bool isEmpty() { return m_things.empty(); }
    bool isBorder() { return m_isBorder; };
    bool isCovered() { return m_covered; };
    bool blockLight() { return m_countFlag.hasNoWalkableEdge && !hasGround(); };
    bool isPathable() { return !m_countFlag.notPathable; }
    bool isDrawable() { return !isEmpty() || !m_walkingCreatures.empty() || !m_effects.empty(); }
    bool isClickable();
    bool isTopGround() const { return m_countFlag.hasTopGround > 0; }
    bool isHouseTile() { return m_houseId != 0 && (m_flags & TILESTATE_HOUSE) == TILESTATE_HOUSE; }
    bool isWalkable(bool ignoreCreatures = false);
    bool isFullGround() { return m_countFlag.fullGround; }
    bool isFullyOpaque() { return isFullGround() || m_countFlag.opaque; }
    bool isLookPossible() { return !m_countFlag.blockProjectile; }
    bool isSingleDimension() { return !m_countFlag.notSingleDimension && m_walkingCreatures.empty(); }
    bool isCompletelyCovered(int8 firstFloor = -1);

    bool hasLight() { return m_countFlag.hasLight; }
    bool hasGround() { return getGround() != nullptr; };
    bool hasCreature() { return m_countFlag.hasCreature; }
    bool hasTopToDraw() const { return m_countFlag.hasTopItem || !m_effects.empty(); }
    bool hasTallThings() { return m_countFlag.hasTallThings; }
    bool hasWideThings() { return m_countFlag.hasWideThings; }
    bool hasDisplacement() { return m_countFlag.hasDisplacement; }
    bool hasGroundToDraw() const { return m_countFlag.hasGroundOrBorder; }
    bool hasBottomToDraw() const { return m_countFlag.hasBottomItem || m_countFlag.hasCommonItem || m_countFlag.hasCreature || !m_walkingCreatures.empty(); }
    bool hasTranslucentLight() { return m_countFlag.hasTransluecentLight; }
    bool hasElevation(int elevation) { return m_countFlag.elevation >= elevation; }
    bool hasThing(const ThingPtr& thing) { return std::find(m_things.begin(), m_things.end(), thing) != m_things.end(); }

    TilePtr asTile() { return static_self_cast<Tile>(); }

private:
    struct CountFlag {
        uint8 fullGround = 0,
            notWalkable = 0,
            notPathable = 0,
            notSingleDimension = 0,
            blockProjectile = 0,
            totalElevation = 0,
            hasDisplacement = 0,
            isNotPathable = 0,
            elevation = 0,
            opaque = 0,
            hasLight = 0,
            hasTallThings = 0,
            hasWideThings = 0,
            hasHookEast = 0,
            hasHookSouth = 0,
            hasTopGround = 0,
            hasNoWalkableEdge = 0,
            hasCreature = 0,
            hasCommonItem = 0,
            hasTopItem = 0,
            hasBottomItem = 0,
            hasGroundOrBorder = 0,
            hasTransluecentLight = 0;
    };

    bool checkForDetachableThing();

    Position m_position;

    uint32 m_flags{ 0 },
        m_houseId{ 0 };

    uint8 m_drawElevation{ 0 },
        m_minimapColor{ 0 };

    std::array<Position, 8> m_positionsAround;

    std::vector<ThingPtr> m_things;
    std::vector<EffectPtr> m_effects;
    std::vector<CreaturePtr> m_walkingCreatures;

    CountFlag m_countFlag;
    Highlight m_highlight;

    stdext::boolean<false> m_covered,
        m_completelyCovered,
        m_isBorder;

    friend class TilePainter;
};

#endif
