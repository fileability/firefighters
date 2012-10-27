#import "physics/collision.hpp"
#import "main/game.hpp"
#import "world/map.hpp"
#import "world/tilephysics.hpp"

void Emitter::update() {
    
    float t = timer.GetElapsedTime();
    float dt = t - lastUpdate;
    if (dt < 1.0 / 100.0)
        return;
    // printf("dt = %f\n", dt);
    lastUpdate = t;
    
    // Delete expired particles
    while (!particles.empty()) {
        const Particle& p = particles.front();
        if (t - p.birthday > p.lifetime || p.dead)
            particles.pop_front();
        else
            break;
    }
    
    // Update particles
    for (Particle& p : particles) {
        if (p.dead) continue;
        
        Vec2<float> v = p.acceleration * dt + p.velocity;
        Vec2<float> r = p.position + (v + p.velocity) * 0.5 * dt;
        
        p.velocity = v;
        p.position = r;
        p.age = t - p.birthday;
        
        // Check for collisions
        Tile tile = worldTileAtPixel(p.position.x, p.position.y);
        // printf("tile at (%d, %d) -> %d -> is solid %d\n", (int)p.position.x, (int)p.position.y, (int)tile, is_solid(tile));
        // printf("  yes it really is %u\n", (unsigned)tile);
        // printf("  %u %u %u %u %u %u %u %u %u\n", (unsigned)Tile::Black, (unsigned)Tile::Dirt, (unsigned)Tile::Grass, (unsigned)Tile::Tarmac, (unsigned)Tile::Pavement, (unsigned)Tile::RoadCenterLine, (unsigned)Tile::BrickWall, (unsigned)Tile::DoorClosedN, (unsigned)Tile::LAST);
        if (is_solid(tile))
            p.dead = true;
    }
    
    // Spawn new particles
    int n = 0;
    if (particleClock.GetElapsedTime() + lastSpawned > 1.0 / spawnFrequency) {
        lastSpawned += particleClock.GetElapsedTime();
        particleClock.Reset();
    
        while (lastSpawned >= 1.0 / spawnFrequency) {
            n++;
            lastSpawned -= 1.0 / spawnFrequency;
        }
    }
    
    if (n == 0)
        return;
    
    for (int i = 0; i < n; i++) {
        Particle p(t, 1.0, Vec2<float>(position.x, position.y));
        
        float theta = normalRealInRange(rng, direction.angle - arc.angle / 2.0, direction.angle + arc.angle / 2.0, 2.0);
        float speed = normalRealInRange(rng, averageSpeed / 2.0, averageSpeed * 3.0 / 2.0, 2.0);
        
        // printf("Random test: %f\n", normalRealInRange(rng, 10.0, 20.0, 2.0));
        // printf("Theta angle: %f | %f => %f\n", direction.angle, arc.angle, theta);
        // printf("Speed: %f => %f\n", averageSpeed, speed);
        
        p.velocity = Vec2<float>::FromPolar(speed, Angle(theta));
        
        // float theta2 = normalRealInRange(rng, 2.0 * theta - direction.angle, direction.angle, 2.0);
        // float accel = normalRealInRange(rng, 2.0 * theta - direction.angle, direction.angle, 2.0);
        p.acceleration = Vec2<float>(0.0, 0.0); //Vec2<float>::FromPolar(- speed, Angle(theta2));
        
        particles.push_back(p);
    }
}
