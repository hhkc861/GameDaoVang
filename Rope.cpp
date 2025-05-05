#include "Rope.h"
#include "constants.h" // For SCREEN_HEIGHT, SCREEN_WIDTH
#include <cmath>       // For sin, cos, M_PI, abs, min, max
#include <vector>      // For vector parameters
#include <algorithm>   // For std::remove_if, std::min, std::max
#include <iostream>    // For debugging (optional)

#ifndef M_PI // Define PI if not already defined by <cmath>
#define M_PI 3.14159265358979323846
#endif

// Constructor implementation
Rope::Rope(SDL_Texture* tex, int thick, int anchorX, int anchorY, int maxLen)
    : GameObject(nullptr, anchorX, anchorY, 1, 0), // Base constructor: null texture, anchor point, width 1, initial height 0
      length(0),
      maxLength(maxLen),
      extendSpeed(8),                 // Speed at which rope extends
      retractSpeedFactorGold(3),      // Retract speed = extendSpeed - factor (min 1)
      retractSpeedFactorStone(5),     // Stone slows retraction more
      isExtending(false),
      isRetracting(false),
      attachedGold(nullptr),
      attachedStone(nullptr),
      angle(0.0),                     // Start angle (straight down)
      angleSpeed(0.025),              // How fast it swings (adjust for desired speed)
      maxAngle(M_PI / 3.5),           // Max swing angle (e.g., ~51 degrees)
      isSwinging(true),               // Start in swinging state
      startX(anchorX),
      startY(anchorY),
      thickness(thick)
{
    // Ensure rect reflects the anchor point (used by base class render if it had a texture)
    rect.x = startX;
    rect.y = startY;
    rect.w = thickness;
    rect.h = 0;
}

// Method to update the anchor point (if player could move)
void Rope::setAnchor(int x, int y) {
    startX = x;
    startY = y;
    rect.x = startX - thickness / 2;
    rect.y = startY;
}

// Method to initiate rope extension
void Rope::extend() {
    // Can only extend if currently swinging and not already extending/retracting
    if (isSwinging && !isExtending && !isRetracting) {
        isExtending = true;
        isRetracting = false;
        isSwinging = false; // Stop swinging
        length = 0;         // Start extension from length 0
        // The current 'angle' is used for the extension direction
    }
}

// Method to initiate rope retraction (usually called internally)
void Rope::retract() {
    isRetracting = true;
    isExtending = false;
    isSwinging = false; // Ensure not swinging while retracting
}

// Main update logic for the rope
void Rope::update(int& score, std::vector<Gold>& golds, std::vector<Stone>& stones) {
    if (isSwinging) {
        // Update angle for swinging motion
        angle += angleSpeed;
        // Reverse direction if max angle is reached
        if (angle > maxAngle || angle < -maxAngle) {
            angleSpeed = -angleSpeed;
            // Clamp angle to prevent exceeding maxAngle due to discrete steps
            angle = std::max(-maxAngle, std::min(angle, maxAngle));
        }
        // Reset state variables that shouldn't persist while swinging
        length = 0;
        attachedGold = nullptr;
        attachedStone = nullptr;

    } else if (isExtending) {
        // Increase rope length
        length += extendSpeed;

        // Calculate current end point
        int endX = startX + static_cast<int>(length * sin(angle));
        int endY = startY + static_cast<int>(length * cos(angle));

        // Check for boundary collision or max length reached
        bool hitBoundary = (endY >= SCREEN_HEIGHT || endX <= 0 || endX >= SCREEN_WIDTH || length >= maxLength);

        if (hitBoundary) {
            // Adjust length precisely to the boundary hit point (optional, but cleaner)
            if (length >= maxLength) length = maxLength;
            // More precise boundary adjustments (requires careful calculation if needed)
            // else if (endY >= SCREEN_HEIGHT) length = ...
            // else if (endX <= 0) length = ...
            // else if (endX >= SCREEN_WIDTH) length = ...

            retract(); // Hit something or reached max length, start retracting
        } else {
            // If still extending within bounds, check for object collision
            // Only check if we haven't already grabbed something on this extension
            if (attachedGold == nullptr && attachedStone == nullptr) {
                attachedGold = checkCollision(golds);
                if (attachedGold != nullptr) {
                    retract(); // Found gold, start retracting
                } else {
                    // Only check for stone if no gold was found
                    attachedStone = checkCollision(stones);
                    if (attachedStone != nullptr) {
                        retract(); // Found stone, start retracting
                    }
                }
            }
        }

    } else if (isRetracting) {
        // Calculate retraction speed based on attached object
        int currentRetractSpeed = extendSpeed; // Start with base speed
        if (attachedGold != nullptr) {
            currentRetractSpeed -= retractSpeedFactorGold;
        } else if (attachedStone != nullptr) {
            currentRetractSpeed -= retractSpeedFactorStone;
        }
        currentRetractSpeed = std::max(1, currentRetractSpeed); // Ensure speed is at least 1

        // Decrease rope length
        length -= currentRetractSpeed;

        // Calculate end point to position the attached object
        int endX = startX + static_cast<int>(length * sin(angle));
        int endY = startY + static_cast<int>(length * cos(angle));

        // Update the position of the attached object to follow the rope end
        if (attachedGold != nullptr) {
            // Center the object on the rope end
            attachedGold->rect.x = endX - attachedGold->rect.w / 2;
            attachedGold->rect.y = endY - attachedGold->rect.h / 2;
        }
        if (attachedStone != nullptr) {
            // Center the object on the rope end
            attachedStone->rect.x = endX - attachedStone->rect.w / 2;
            attachedStone->rect.y = endY - attachedStone->rect.h / 2;
        }

        // Check if rope is fully retracted
        if (length <= 0) {
            length = 0;
            isRetracting = false;

            // Process collected object
            if (attachedGold != nullptr) {
                score += attachedGold->value; // Add value to score
                // Remove the collected gold object from the main vector
                golds.erase(std::remove_if(golds.begin(), golds.end(),
                                       [&](const Gold& g) { return &g == attachedGold; }),
                            golds.end());
                attachedGold = nullptr; // Release the pointer
            } else if (attachedStone != nullptr) {
                score += attachedStone->value; // Add value to score
                // Remove the collected stone object from the main vector
                stones.erase(std::remove_if(stones.begin(), stones.end(),
                                        [&](const Stone& s) { return &s == attachedStone; }),
                             stones.end());
                attachedStone = nullptr; // Release the pointer
            }

            // Finished retracting, start swinging again
            isSwinging = true;
            // Keep the current angle and angleSpeed for the next swing cycle
        }
    }
}

// Render the rope and any attached object
void Rope::render(SDL_Renderer* renderer) const {
    // Xác định chiều dài để vẽ
    int drawLength = length;
    if (isSwinging) {
        // Vẫn vẽ một đoạn ngắn khi đang đu đưa để thấy dây
        drawLength = 30;
    }

    // Nếu chiều dài vẽ quá ngắn, không cần vẽ nhiều đường phức tạp
    if (drawLength < 1) {
        // Có thể chọn không vẽ gì, hoặc vẽ một dấu chấm/đường ngắn tại điểm neo
        if (thickness > 0) { // Chỉ vẽ nếu có độ dày
             SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Màu nâu
             // Vẽ một đường dọc ngắn tại điểm neo với độ dày mong muốn
             int halfThickFloor = thickness / 2;
             int startDrawX = startX - halfThickFloor;
             SDL_Rect thickAnchorRect = {startDrawX, startY, thickness, 2}; // Hình chữ nhật nhỏ dày
             SDL_RenderFillRect(renderer, &thickAnchorRect); // Vẽ hình chữ nhật này
        }
        // Vẫn render vật thể đính kèm ngay cả khi dây bằng 0 (nó sẽ ở điểm neo)
        if (attachedGold != nullptr) attachedGold->render(renderer);
        if (attachedStone != nullptr) attachedStone->render(renderer);
        return; // Kết thúc render sớm
    }

    // Tính toán điểm cuối dựa trên góc và chiều dài vẽ
    // Sử dụng double cho tính toán trung gian để chính xác hơn
    double endX_d = static_cast<double>(startX) + drawLength * sin(angle);
    double endY_d = static_cast<double>(startY) + drawLength * cos(angle);

    // Vector hướng của dây
    double dx = endX_d - startX;
    double dy = endY_d - startY;

    // Độ dài (khoảng cách) thực tế của đoạn dây vẽ
    double magnitude = std::sqrt(dx * dx + dy * dy);

    // Đặt màu vẽ cho dây
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Màu nâu

    // Xử lý trường hợp dây quá ngắn hoặc điểm đầu cuối trùng nhau (tránh chia cho 0)
    if (magnitude < 0.001) {
         // Vẽ một đường dọc ngắn tại điểm neo như trường hợp drawLength < 1
         if (thickness > 0) {
            int halfThickFloor = thickness / 2;
            int startDrawX = startX - halfThickFloor;
            SDL_Rect thickAnchorRect = {startDrawX, startY, thickness, std::max(1, drawLength)}; // Dài tối thiểu 1 px
            SDL_RenderFillRect(renderer, &thickAnchorRect);
         }
    } else {
        // Tính toán vector pháp tuyến đơn vị (vuông góc với dây)
        // Vector vuông góc: (-dy, dx)
        // Vector pháp tuyến đơn vị: (-dy/magnitude, dx/magnitude)
        double perpX = -dy / magnitude;
        double perpY = dx / magnitude;

        // Tính toán vị trí bắt đầu lệch để các đường thẳng được căn giữa
        // Ví dụ: thickness = 4 -> các đường ở vị trí -1.5, -0.5, +0.5, +1.5 so với đường trung tâm
        double startOffsetFactor = -(static_cast<double>(thickness) - 1.0) / 2.0;

        // Vẽ 'thickness' đường thẳng song song
        for (int i = 0; i < thickness; ++i) {
            // Tính độ lệch (offset) cho đường thẳng hiện tại so với đường trung tâm
            double currentOffsetFactor = startOffsetFactor + i;
            double offsetX = perpX * currentOffsetFactor;
            double offsetY = perpY * currentOffsetFactor;

            // Tính tọa độ điểm đầu và cuối của đường thẳng song song này
            // Sử dụng round() để làm tròn tới số nguyên gần nhất, giúp các đường thẳng đều hơn
            int lineStartX = static_cast<int>(std::round(startX + offsetX));
            int lineStartY = static_cast<int>(std::round(startY + offsetY));
            int lineEndX = static_cast<int>(std::round(endX_d + offsetX));
            int lineEndY = static_cast<int>(std::round(endY_d + offsetY));

            // Vẽ đường thẳng
            SDL_RenderDrawLine(renderer, lineStartX, lineStartY, lineEndX, lineEndY);
        }
    }

    // Render vật thể đính kèm (nếu có) - vị trí đã được cập nhật trong hàm update()
    if (attachedGold != nullptr) {
        attachedGold->render(renderer);
    }
    if (attachedStone != nullptr) {
        attachedStone->render(renderer);
    }
}

// Check for collision between the rope tip and gold objects
Gold* Rope::checkCollision(std::vector<Gold>& golds) {
    // Collision only makes sense when extending
    if (!isExtending) return nullptr;

    // Calculate the rope's current end point
    int endX = startX + static_cast<int>(length * sin(angle));
    int endY = startY + static_cast<int>(length * cos(angle));

    // Define a small rectangle around the rope tip for collision detection
    // Adjust size (e.g., 10x10) as needed for sensitivity
    SDL_Rect ropeTipRect = {endX - 5, endY - 5, 10, 10};

    // Iterate through gold objects and check for intersection
    for (Gold& gold : golds) {
        SDL_Rect intersection; // Output rect for intersection details (optional)
        // Check if the rope tip rectangle overlaps with the gold's rectangle
        if (SDL_IntersectRect(&ropeTipRect, &gold.rect, &intersection)) {
            return &gold; // Return a pointer to the collided gold object
        }
    }

    return nullptr; // No collision detected
}

// Check for collision between the rope tip and stone objects
Stone* Rope::checkCollision(std::vector<Stone>& stones) {
    // Collision only makes sense when extending
    if (!isExtending) return nullptr;

    // Calculate the rope's current end point
    int endX = startX + static_cast<int>(length * sin(angle));
    int endY = startY + static_cast<int>(length * cos(angle));

    // Define a small rectangle around the rope tip
    SDL_Rect ropeTipRect = {endX - 5, endY - 5, 20, 10};

    // Iterate through stone objects and check for intersection
    for (Stone& stone : stones) {
        SDL_Rect intersection;
        if (SDL_IntersectRect(&ropeTipRect, &stone.rect, &intersection)) {
            return &stone; // Return a pointer to the collided stone object
        }
    }

    return nullptr; // No collision detected
}
