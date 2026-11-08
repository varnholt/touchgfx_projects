#include "gui/common/toruseffect.hpp"
#include "gui/common/cameraprojection.hpp"
#include "gui/common/effectsurface.hpp"
#include "gui/common/poly.hpp"
#include <touchgfx/Application.hpp>
#include <algorithm>
#include <cmath>

Vector3D TorusEffect::transform(const Vector3D& point, const Matrix& matrix)
{
    return matrix * point;
}

TorusEffect::TorusEffect()
{
    setWidth(touchgfx::HAL::DISPLAY_WIDTH);
    setHeight(touchgfx::HAL::DISPLAY_HEIGHT);
    touchgfx::Application::getInstance()->registerTimerWidget(this);
    setTouchable(true);
}

TorusEffect::~TorusEffect()
{
    touchgfx::Application::getInstance()->unregisterTimerWidget(this);
}

touchgfx::Rect TorusEffect::getSolidRect() const { return touchgfx::Rect(0, 0, 0, 0); }

void TorusEffect::draw(const touchgfx::Rect& invalidatedArea) const
{
    if (_environmentMap == touchgfx::BITMAP_INVALID) return;
    const touchgfx::Bitmap texture(_environmentMap);
    if (texture.getFormat() != touchgfx::Bitmap::RGB565 || texture.getWidth() != 256 ||
        texture.getHeight() != 256) return;
    const EffectSurface frame(*this, invalidatedArea);
    const auto surface = frame.surface();
    if (surface.clip().empty()) return;

    const Matrix object = Matrix::rotateY(_rotationX) * Matrix::rotateX(_rotationY);
    Matrix camera;
    camera.setTranslation(-_dx * 6, static_cast<float>(std::sin(_time * 2) * 5) - _dy * 6, 36 + _dy);
    const Matrix combined = object * camera;
    for (int i = 0; i < _mesh->getNumVertices(); ++i)
    {
        _cameraVertices[i] = transform(_mesh->getVertices()[i], combined);
        _environmentUV[i] = transform(_mesh->getNormals()[i], object);
    }

    const int32_t* indices = _mesh->getTriIndices();
    int faceCount = 0;
    for (int face = 0; face < _mesh->getNumTris(); ++face)
    {
        const int i = face * 3;
        const float z0 = _cameraVertices[indices[i]].z;
        const float z1 = _cameraVertices[indices[i + 1]].z;
        const float z2 = _cameraVertices[indices[i + 2]].z;
        if (z0 < 1 && z1 < 1 && z2 < 1) continue;
        _faces[faceCount++] = {(z0 + z1 + z2) / 3, i};
    }
    std::sort(_faces.begin(), _faces.begin() + faceCount, [](const Face& a, const Face& b)
    {
        return a.depth != b.depth ? a.depth > b.depth : a.index < b.index;
    });
    const auto* pixels = reinterpret_cast<const uint16_t*>(texture.getData());
    for (int face = 0; face < faceCount; ++face)
    {
        CameraVertex triangle[3];
        for (int corner = 0; corner < 3; ++corner)
        {
            const int index = indices[_faces[face].index + corner];
            const auto& uv = _environmentUV[index];
            triangle[corner] = {_cameraVertices[index], (128 + uv.x * 127) * 65536,
                               (128 + uv.y * 127) * 65536};
        }
        CameraVertex clipped[4];
        const int count = clipNearPlane(triangle, clipped, 1.0f);
        if (count < 3) continue;
        const Vertex a = projectCameraVertex(clipped[0], getWidth(), getHeight());
        for (int corner = 1; corner + 1 < count; ++corner)
        {
            const Vertex b = projectCameraVertex(clipped[corner], getWidth(), getHeight());
            const Vertex c = projectCameraVertex(clipped[corner + 1], getWidth(), getHeight());
            const float winding = (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
            if (winding < 0) drawTriangle(surface, a, b, c, pixels);
        }
    }
}

void TorusEffect::handleTickEvent()
{
    const auto input = _input.consume();
    // Bound camera displacement so repeated input cannot move the mesh through the eye.
    _dx = (std::max)(-1.5f, (std::min)(1.5f, _dx - input.dx * 0.0025f));
    _dy = (std::max)(-1.5f, (std::min)(1.5f, _dy - input.dy * 0.0025f));
    _rotationX = std::fmod(_rotationX + _dx, 6.2831853f);
    _rotationY = std::fmod(_rotationY + _dy, 6.2831853f);
    _dx *= 0.95f;
    _dy *= 0.95f;
    _time = std::fmod(_time + 0.01f, 6.2831853f);
    if (_idle_ticks < 5001) ++_idle_ticks;
    if (_idle_ticks > 5000) { _rotationX *= 0.99f; _rotationY *= 0.99f; }
    invalidate();
}

void TorusEffect::handleClickEvent(const touchgfx::ClickEvent& event)
{
    if (event.getType() == touchgfx::ClickEvent::PRESSED)
    {
        _input.press(event.getX(), event.getY(), getWidth(), getHeight());
        if (_input.active()) _idle_ticks = 0;
    }
    else if (event.getType() == touchgfx::ClickEvent::CANCEL) _input.cancel();
    else _input.release();
}

void TorusEffect::handleDragEvent(const touchgfx::DragEvent& event)
{
    if (!_input.active()) return;
    _input.drag(event.getOldX(), event.getOldY(), event.getNewX(), event.getNewY(), getWidth(), getHeight());
    _idle_ticks = 0;
}

void TorusEffect::setEnvMapTexture(touchgfx::BitmapId id) { _environmentMap = id; }
void TorusEffect::setLowQuality() { _mesh = &_low; }
void TorusEffect::setMediumQuality() { _mesh = &_medium; }
void TorusEffect::setHighQuality() { _mesh = &_high; }
