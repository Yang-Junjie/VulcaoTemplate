#include <array>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>

#include <vulcao/command_buffer.h>
#include <vulcao/context.h>
#include <vulcao/frame_manager.h>
#include <vulcao/rendering.h>
#include <vulcao/surface.h>
#include <vulcao/window_presenter.h>

namespace {

constexpr int kInitialWidth = 1280;
constexpr int kInitialHeight = 720;

struct SdlScope {
    SdlScope() {
        if (!SDL_Init(SDL_INIT_VIDEO))
            throw std::runtime_error(std::string{"SDL_Init failed: "} + SDL_GetError());
    }

    ~SdlScope() { SDL_Quit(); }

    SdlScope(const SdlScope&) = delete;
    SdlScope& operator=(const SdlScope&) = delete;
};

struct Window {
    explicit Window(int width, int height) {
        handle = SDL_CreateWindow("VulcaoTemplate", width, height,
                                  SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE |
                                      SDL_WINDOW_HIGH_PIXEL_DENSITY);
        if (handle == nullptr)
            throw std::runtime_error(std::string{"SDL_CreateWindow failed: "} + SDL_GetError());
    }

    ~Window() {
        if (handle != nullptr)
            SDL_DestroyWindow(handle);
    }

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    SDL_Window* handle = nullptr;
};

vk::Extent2D pixel_extent(SDL_Window* window) {
    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    if (width <= 0 || height <= 0)
        return {};
    return vk::Extent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

bool same_extent(vk::Extent2D lhs, vk::Extent2D rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

} // namespace

int main(int /*argc*/, char* /*argv*/[]) {
    try {
        SdlScope sdl;
        Window window{kInitialWidth, kInitialHeight};

        Uint32 extension_count = 0;
        const char* const* sdl_extensions = SDL_Vulkan_GetInstanceExtensions(&extension_count);
        if (sdl_extensions == nullptr)
            throw std::runtime_error(std::string{"SDL_Vulkan_GetInstanceExtensions failed: "} +
                                     SDL_GetError());

        vulcao::ContextInfo context_info{
            .app_name = "VulcaoTemplate",
            .extensions = std::vector<const char*>(sdl_extensions,
                                                   sdl_extensions + extension_count),
        };
        vulcao::Context context{context_info};

        VkSurfaceKHR raw_surface = VK_NULL_HANDLE;
        if (!SDL_Vulkan_CreateSurface(window.handle, static_cast<VkInstance>(context.instance()),
                                      nullptr, &raw_surface))
            throw std::runtime_error(std::string{"SDL_Vulkan_CreateSurface failed: "} + SDL_GetError());

        vulcao::Surface surface =
            vulcao::Surface::adopt(context.instance(), vk::SurfaceKHR{raw_surface});
        context.initialize(surface.handle());

        const vk::Extent2D initial_extent = pixel_extent(window.handle);
        if (initial_extent.width == 0 || initial_extent.height == 0)
            throw std::runtime_error("the window has no drawable extent");

        vulcao::WindowPresenter presenter{
            context, surface, vulcao::WindowPresenterInfo{.extent = initial_extent}};
        vulcao::FrameManager frames{context};

        const vk::ClearColorValue clear_color{
            std::array<float, 4>{0.05f, 0.08f, 0.12f, 1.0f}};

        bool running = true;
        while (running) {
            SDL_Event event{};
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT)
                    running = false;
                else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
                    running = false;
            }
            if (!running)
                break;

            const vk::Extent2D requested_extent = pixel_extent(window.handle);
            if (requested_extent.width == 0 || requested_extent.height == 0) {
                SDL_Delay(1);
                continue;
            }
            if (!same_extent(requested_extent, presenter.extent())) {
                if (!presenter.recreate(requested_extent)) {
                    SDL_Delay(1);
                    continue;
                }
            }

            vulcao::Frame frame = frames.begin_frame();
            const auto acquired = presenter.acquire(frame);
            if (!acquired) {
                frames.submit(frame);
                frames.end_frame(frame);
                presenter.recreate(requested_extent);
                continue;
            }

            const uint32_t image_index = acquired->image_index;
            vulcao::CommandBuffer& cmd = *frame.command_buffer;
            const vk::Image image = presenter.swapchain().images()[image_index];
            const vk::ImageView view = presenter.swapchain().image_views()[image_index];

            cmd.transition_to_render(image);
            cmd.begin_rendering(
                presenter.extent(),
                vulcao::color_attachment(view, vk::ImageLayout::eColorAttachmentOptimal,
                                         clear_color));
            cmd.end_rendering();
            cmd.transition_to_present(image);

            const std::array waits{presenter.acquire_wait(frame)};
            const std::array signals{presenter.render_finished(image_index)};
            frames.submit(frame, waits, signals);
            frames.end_frame(frame);

            const bool presented = presenter.present(presenter.present_queue(), image_index);
            if (!presented || acquired->suboptimal) {
                const vk::Extent2D current_extent = pixel_extent(window.handle);
                if (current_extent.width != 0 && current_extent.height != 0)
                    presenter.recreate(current_extent);
            }
        }
    } catch (const std::exception& error) {
        std::cerr << "fatal: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}
