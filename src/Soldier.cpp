#include "Soldier.h"
#include "GameUtils.h"

Soldier::Soldier()
    : home(0.f, 0.f)
    , position(0.f, 0.f)
    , targetPos(0.f, 0.f)
{
    animSprite.frames = { "soldier_running_0", "soldier_running_1", "soldier_running_2", "soldier_running_3", "soldier_running_4", "soldier_running_5", "soldier_running_6", "soldier_running_7"}; 
    animSprite.fps = 8.f; 
        { 
            sf::Texture& t = SpriteManager::get().getTexture("soldier_running_0"); 
            sf::Vector2u sz = t.getSize(); animSprite.sprite.setTexture(t, true); 
            animSprite.sprite.setOrigin(sz.x / 2.f, sz.y); 
            animSprite.sprite.setScale(1.f, 1.f); 
        }

    


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

void setIdleAnimation(AnimatedSprite& animSprite)
{
    if (!animSprite.frames.empty() &&
        animSprite.frames[0] == "soldier_idle")
        return;

    animSprite.frames = { "soldier_idle" };

    animSprite.frameIndex = 0;
    animSprite.fps = 1.f;

    animSprite.sprite.setTexture(
        SpriteManager::get().getTexture("soldier_idle"),
        true
    );
}

void setAttackAnimation(AnimatedSprite& animSprite)
{
    if (!animSprite.frames.empty() &&
        animSprite.frames[0] == "soldier_attack_0")
        return;

    animSprite.frames = {
        "soldier_attack_0",
        "soldier_attack_1",
        "soldier_attack_2",
        "soldier_attack_3"
    };

    animSprite.frameIndex = 0;
    animSprite.fps = 4.f;

    animSprite.sprite.setTexture(
        SpriteManager::get().getTexture("soldier_attack_0"),
        true
    );
}
void setRunningAnimation(AnimatedSprite& animSprite)
{
    if (!animSprite.frames.empty() &&
        animSprite.frames[0] == "soldier_running_0")
        return;

    animSprite.frames = {
        "soldier_running_0",
        "soldier_running_1",
        "soldier_running_2",
        "soldier_running_3",
        "soldier_running_4",
        "soldier_running_5",
        "soldier_running_6",
        "soldier_running_7"
    };

    animSprite.frameIndex = 0;
    animSprite.fps = 8.f;

    animSprite.sprite.setTexture(
        SpriteManager::get().getTexture("soldier_running_0"),
        true
    );
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
        }
        return;
    }

    // Move toward ordered position
    sf::Vector2f diff = targetPos - position;
    float d = dist(targetPos, position);
    isMoving = false;

if (d > 2.f) {
    isMoving = true;
    position += diff / d * moveSpeed * dt;
    if (diff.x < 0.f)
        facingLeft = true;
    else
        facingLeft = false;
    if (!isAttacking)
    setRunningAnimation(animSprite);
}

    // Auto-attack nearest enemy in range
    if (attackTimer > 0.f) attackTimer -= dt;
    if (attackAnimTimer > 0.f){
        attackAnimTimer -= dt;
        isAttacking = true;}
    else
    {isAttacking = false;}

    if (attackTimer <= 0.f) {
        Enemy* target  = nullptr;
        float  minDist = attackRange;
        for (auto& e : enemies) {
            if (!e.isAlive()) continue;
            float d_dist = dist(e.getPosition(), position);
            if (d_dist < minDist) { minDist = d_dist; target = &e; }
        }
        if (target) {
            isAttacking = true;
            attackAnimTimer = 0.4f;
            setAttackAnimation(animSprite);
            target->takeDamage(damage);
            attackTimer = attackCooldown;
        }
    }

    if (!isMoving && !isAttacking)
    {setIdleAnimation(animSprite);}

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

    animSprite.update(dt);
    animSprite.sprite.setPosition(position);

    if (facingLeft)
    animSprite.sprite.setScale(-1.f, 1.f);
    else
    animSprite.sprite.setScale(1.f, 1.f);
}

void Soldier::draw(sf::RenderWindow& window) {
    if (state == SoldierState::Inactive) return;

    if (state == SoldierState::Dead) {
        // Ghost to show respawn is pending
        animSprite.sprite.setPosition(home.x, home.y + SPRITE_Y_OFFSET);
        animSprite.sprite.setColor(sf::Color(255,255,255,80));
        window.draw(animSprite.sprite);
        animSprite.sprite.setColor(sf::Color::White);
        return;
    }

    window.draw(animSprite.sprite);
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
