#include "Soldier.h"
#include "GameUtils.h"

Soldier::Soldier()
    : home(0.f, 0.f)
    , position(0.f, 0.f)
    , targetPos(0.f, 0.f)
{
    shape.setRadius(6.f);
    shape.setOrigin(6.f, 6.f);
    shape.setFillColor(sf::Color(220, 180, 60));
    shape.setOutlineColor(sf::Color(255, 230, 100));
    shape.setOutlineThickness(1.5f);

    patrolCircle.setRadius(PATROL_RADIUS);
    patrolCircle.setOrigin(PATROL_RADIUS, PATROL_RADIUS);
    patrolCircle.setFillColor(sf::Color(255, 215, 0, 15));
    patrolCircle.setOutlineColor(sf::Color(255, 215, 0, 80));
    patrolCircle.setOutlineThickness(1.f);
}

void Soldier::activate(sf::Vector2f homePos) {
    home      = homePos;
    position  = homePos;
    targetPos = homePos;
    hp        = maxHp;
    state     = SoldierState::Alive;
    shape.setPosition(position);
    patrolCircle.setPosition(home);
}

void Soldier::deactivate() {
    state = SoldierState::Inactive;
}

void Soldier::orderMoveTo(sf::Vector2f worldPos) {
    if (state != SoldierState::Alive) return;
    sf::Vector2f diff = worldPos - home;
    float d = dist(worldPos, home);
    if (d > PATROL_RADIUS)
        diff = diff / d * PATROL_RADIUS;
    targetPos = home + diff;
}

void Soldier::update(float dt, std::vector<Enemy>& enemies) {
    if (state == SoldierState::Inactive) return;

    if (state == SoldierState::Dead) {
        respawnTimer -= dt;
        if (respawnTimer <= 0.f) {
            position  = home;
            targetPos = home;
            hp        = maxHp;
            state     = SoldierState::Alive;
            shape.setPosition(position);
        }
        return;
    }

    // Move toward ordered position
    sf::Vector2f diff = targetPos - position;
    float d = dist(targetPos, position);
    if (d > 2.f) {
        position += diff / d * moveSpeed * dt;
        shape.setPosition(position);
    }

    // Auto-attack nearest enemy in range
    if (attackTimer > 0.f) attackTimer -= dt;
    if (attackTimer <= 0.f) {
        Enemy* target  = nullptr;
        float  minDist = attackRange;
        for (auto& e : enemies) {
            if (!e.isAlive()) continue;
            float d_dist = dist(e.getPosition(), position);
            if (d_dist < minDist) { minDist = d_dist; target = &e; }
        }
        if (target) {
            target->takeDamage(damage);
            attackTimer = attackCooldown;
        }
    }

    // Take passive damage when enemies are in contact
    for (auto& e : enemies) {
        if (!e.isAlive()) continue;
        if (dist(e.getPosition(), position) < 14.f) {
            hp -= e.getDamage() * 0.4f * dt;
        }
    }
    if (hp <= 0.f) {
        state        = SoldierState::Dead;
        respawnTimer = RESPAWN_TIME;
    }
}

void Soldier::draw(sf::RenderWindow& window) {
    if (state == SoldierState::Inactive) return;

    if (state == SoldierState::Dead) {
        // Ghost to show respawn is pending
        sf::CircleShape ghost(6.f);
        ghost.setOrigin(6.f, 6.f);
        ghost.setPosition(home);
        ghost.setFillColor(sf::Color(220, 180, 60, 50));
        ghost.setOutlineColor(sf::Color(255, 230, 100, 70));
        ghost.setOutlineThickness(1.5f);
        window.draw(ghost);
        return;
    }

    window.draw(shape);
    drawHpBar(window);
}

void Soldier::drawPatrolRadius(sf::RenderWindow& window) {
    if (state == SoldierState::Inactive) return;
    window.draw(patrolCircle);
}

void Soldier::drawHpBar(sf::RenderWindow& window) {
    drawHealthBar(window, position.x - 6.f, position.y - 9.f,
                  12.f, 3.f, hp / maxHp, sf::Color(80, 220, 80));
}

bool         Soldier::isActive()     const { return state != SoldierState::Inactive; }
sf::Vector2f Soldier::getPosition()  const { return position; }
sf::Vector2f Soldier::getHome()      const { return home; }

bool Soldier::containsPoint(sf::Vector2f p) const {
    if (state != SoldierState::Alive) return false;
    sf::Vector2f d = p - position;
    return std::sqrt(d.x * d.x + d.y * d.y) < 8.f;
}
