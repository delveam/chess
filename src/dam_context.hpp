#ifndef DAM_CONTEXT_HPP
#define DAM_CONTEXT_HPP
#include <any>
#include <string>
#include <memory>

class DamContext {
public:
    template <typename TEXTURE>
    TEXTURE load_texture(std::string path)
    {
        std::any texture = load_texture_impl(path);
        return std::any_cast<TEXTURE>(texture);
    }
    template <typename FONT>
    FONT load_font(std::string path)
    {
        std::any texture = load_font_impl(path);
        return std::any_cast<FONT>(texture);
    }
    template <typename COLOR>
    void draw_rectangle(float x, float y, float width, float height, COLOR color)
    {
        draw_rectangle_impl(x, y, width, height, std::any(color));
    }
    template <typename TEXTURE>
    void draw_texture(float x, float y, TEXTURE texture)
    {
        draw_texture_impl(x, y, std::any(texture));
    }
    template <typename FONT, typename COLOR>
    void draw_text(float x, float y, std::string text, FONT* font, COLOR color)
    {
        draw_text_impl(x, y, text, std::any(font), std::any(color));
    }
    template <typename TEXTURE>
    void unload_texture(TEXTURE texture)
    {
        unload_texture_impl(std::any(texture));
    }
    template <typename FONT>
    void unload_font(FONT font)
    {
        unload_font_impl(std::any(font));
    }
    template <typename KEYBOARD_STATE>
    KEYBOARD_STATE get_keyboard_state_bruh(std::any state)
    {
        std::any state2 = get_keyboard_state_impl(state);
        return std::any_cast<KEYBOARD_STATE>(state2);
    }
protected:
    virtual std::any load_texture_impl(std::string path) = 0;
    virtual std::any load_font_impl(std::string path) = 0;
    virtual void draw_rectangle_impl(float x, float y, float width, float height, std::any color) = 0;
    virtual void draw_texture_impl(float x, float y, std::any texture) = 0;
    virtual void draw_text_impl(float x, float y, std::string text, std::any font, std::any color) = 0;
    virtual void unload_texture_impl(std::any texture) = 0;
    virtual void unload_font_impl(std::any font) = 0;
    virtual std::any get_keyboard_state_impl(std::any state) = 0;
};
#endif
