#include "Rope.h"
#include "constants.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

Rope::Rope(SDL_Texture* tex, int thick, int anchorX, int anchorY, int maxLen)
    : GameObject(nullptr, anchorX, anchorY, 1, 0),
      length(0),
      maxLength(maxLen),
      extendSpeed(8),
      retractSpeedFactorGold(3),
      retractSpeedFactorStone(5),
      isExtending(false),
      isRetracting(false),
      attachedGold(nullptr),
      attachedStone(nullptr),
      angle(0.0),
      angleSpeed(0.025),
      maxAngle(M_PI / 2.5),
      isSwinging(true),
      startX(anchorX),
      startY(anchorY),
      thickness(thick)
{

    rect.x = startX;
    rect.y = startY;
    rect.w = thickness;
    rect.h = 0;
}

void Rope::setAnchor(int x, int y) {
    startX = x;
    startY = y;
    rect.x = startX - thickness / 2;
    rect.y = startY;
}

void Rope::extend() {
    if (isSwinging && !isExtending && !isRetracting) {
        isExtending = true;
        isRetracting = false;
        isSwinging = false;
        length = 0;         // Start extension from length 0
    }
}

void Rope::retract() {
    isRetracting = true;
    isExtending = false;
    isSwinging = false;
}

void Rope::update(int& score, std::vector<Gold>& golds, std::vector<Stone>& stones) {
    if (isSwinging) {
        angle += angleSpeed;
        if (angle > maxAngle || angle < -maxAngle) {
            angleSpeed = -angleSpeed;
            angle = std::max(-maxAngle, std::min(angle, maxAngle));
        }
        length = 0;
        attachedGold = nullptr;
        attachedStone = nullptr;

    } else if (isExtending) {
        length += extendSpeed; // Tăng chiều dài dây theo tốc độ vươn
        // Tính tọa độ đầu dây (endX, endY) dựa trên góc và chiều dài
        int endX = startX + static_cast<int>(length * sin(angle)); // X = startX + length * sin(góc)
        int endY = startY + static_cast<int>(length * cos(angle)); // Y = startY + length * cos(góc)
                                                                  // (cos cho Y vì góc 0 là thẳng xuống)
        bool hitBoundary = (endY >= SCREEN_HEIGHT || endX <= 0 || endX >= SCREEN_WIDTH || length >= maxLength);

        if (hitBoundary) {
            if (length >= maxLength) length = maxLength;
            retract();
        } else {
            if (attachedGold == nullptr && attachedStone == nullptr) {
                attachedGold = checkCollision(golds);
                if (attachedGold != nullptr) {
                    retract();
                } else {
                    attachedStone = checkCollision(stones);
                    if (attachedStone != nullptr) {
                        retract();
                    }
                }
            }
        }

    } else if (isRetracting) {
        int currentRetractSpeed = extendSpeed;
        if (attachedGold != nullptr) {
            currentRetractSpeed -= retractSpeedFactorGold;
        } else if (attachedStone != nullptr) {
            currentRetractSpeed -= retractSpeedFactorStone;
        }
        currentRetractSpeed = std::max(1, currentRetractSpeed);
        length -= currentRetractSpeed;

        int endX = startX + static_cast<int>(length * sin(angle));
        int endY = startY + static_cast<int>(length * cos(angle));

        if (attachedGold != nullptr) {
            attachedGold->rect.x = endX - attachedGold->rect.w / 2;
            attachedGold->rect.y = endY - attachedGold->rect.h / 2;
        }
        if (attachedStone != nullptr) {
            attachedStone->rect.x = endX - attachedStone->rect.w / 2;
            attachedStone->rect.y = endY - attachedStone->rect.h / 2;
        }

        if (length <= 0) {
            length = 0;
            isRetracting = false;

            if (attachedGold != nullptr) {
                score += attachedGold->value;
                golds.erase(std::remove_if(golds.begin(), golds.end(),
                                       [&](const Gold& g) { return &g == attachedGold; }),
                            golds.end());
                attachedGold = nullptr;
            } else if (attachedStone != nullptr) {
                score += attachedStone->value;
                stones.erase(std::remove_if(stones.begin(), stones.end(),
                                        [&](const Stone& s) { return &s == attachedStone; }),
                             stones.end());
                attachedStone = nullptr;
            }
            isSwinging = true;
        }
    }
}

void Rope::render(SDL_Renderer* renderer) const {
    int drawLength = length;
    if (isSwinging) {
        drawLength = 30;
    }

    if (drawLength < 1) {
        if (thickness > 0) { // Chỉ vẽ nếu có độ dày
             SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
             int halfThickFloor = thickness / 2;
             int startDrawX = startX - halfThickFloor;
             SDL_Rect thickAnchorRect = {startDrawX, startY, thickness, 2}; // Hình chữ nhật nhỏ dày
             SDL_RenderFillRect(renderer, &thickAnchorRect); // Vẽ hình chữ nhật này
        }
        if (attachedGold != nullptr) attachedGold->render(renderer);
        if (attachedStone != nullptr) attachedStone->render(renderer);
        return; // Kết thúc render sớm
    }

    // Tính tọa độ điểm cuối của dây (kiểu double để chính xác hơn)
    double endX_d = static_cast<double>(startX) + drawLength * sin(angle);
    double endY_d = static_cast<double>(startY) + drawLength * cos(angle);

    // Vector hướng của dây (từ điểm đầu đến điểm cuối)
    double dx = endX_d - startX; // Thay đổi theo trục X
    double dy = endY_d - startY; // Thay đổi theo trục Y

    double magnitude = std::sqrt(dx * dx + dy * dy);

    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Màu nâu

    if (magnitude < 0.001) {
         if (thickness > 0) {
            int halfThickFloor = thickness / 2;
            int startDrawX = startX - halfThickFloor;
            SDL_Rect thickAnchorRect = {startDrawX, startY, thickness, std::max(1, drawLength)}; // Dài tối thiểu 1 px
            SDL_RenderFillRect(renderer, &thickAnchorRect);
         }
    } else {
        // Tính vector pháp tuyến (vuông góc với vector hướng của dây) để tạo độ dày
        double perpX = -dy / magnitude; // Component X của vector pháp tuyến đơn vị
        double perpY = dx / magnitude;  // Component Y của vector pháp tuyến đơn vị

        // Tính toán offset để các đường song song tạo thành độ dày được căn giữa
        double startOffsetFactor = -(static_cast<double>(thickness) - 1.0) / 2.0;

        for (int i = 0; i < thickness; ++i) {
             double currentOffsetFactor = startOffsetFactor + i; // Offset cho đường thẳng hiện tại
            // Tính toán độ lệch (offset) dựa trên vector pháp tuyến và offset factor
            double offsetX = perpX * currentOffsetFactor;
            double offsetY = perpY * currentOffsetFactor;

              // Tọa độ bắt đầu của đường thẳng (sợi dây) hiện tại, đã áp dụng offset
            int lineStartX = static_cast<int>(std::round(startX + offsetX));
            int lineStartY = static_cast<int>(std::round(startY + offsetY));
            // Tọa độ kết thúc của đường thẳng (sợi dây) hiện tại, đã áp dụng offset
            int lineEndX = static_cast<int>(std::round(endX_d + offsetX));
            int lineEndY = static_cast<int>(std::round(endY_d + offsetY));

            // Vẽ đường thẳng
            SDL_RenderDrawLine(renderer, lineStartX, lineStartY, lineEndX, lineEndY);
        }
    }

    if (attachedGold != nullptr) {
        attachedGold->render(renderer);
    }
    if (attachedStone != nullptr) {
        attachedStone->render(renderer);
    }
}

Gold* Rope::checkCollision(std::vector<Gold>& golds) {
    if (!isExtending) return nullptr;

    int endX = startX + static_cast<int>(length * sin(angle));
    int endY = startY + static_cast<int>(length * cos(angle));

    SDL_Rect ropeTipRect = {endX - 5, endY - 5, 10, 10};

    for (Gold& gold : golds) {
        SDL_Rect intersection;
        if (SDL_IntersectRect(&ropeTipRect, &gold.rect, &intersection)) {
            return &gold;
        }
    }

    return nullptr;
}

Stone* Rope::checkCollision(std::vector<Stone>& stones) {

    if (!isExtending) return nullptr;

    //toạ độ đầu dây
    int endX = startX + static_cast<int>(length * sin(angle));
    int endY = startY + static_cast<int>(length * cos(angle));

    SDL_Rect ropeTipRect = {endX - 5, endY - 5, 20, 10};

    for (Stone& stone : stones) {
        SDL_Rect intersection;
        if (SDL_IntersectRect(&ropeTipRect, &stone.rect, &intersection)) {
            return &stone;
        }
    }

    return nullptr;
}
