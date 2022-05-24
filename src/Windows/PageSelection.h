#pragma once

#include "Component.h"
#include "Model/Page.h"
#include "UiDelegate.h"

class PageSelection : public Component
{
public:
    PageSelection(Pages pages, uint8_t newActivePage, UiDelegate *newDelegate);
    ~PageSelection() override;

    void paint(Graphics &g) override;
    void resized(void) override;

private:
    void setActivePageLabel(uint8_t newActivePage);
    static void paintTitleBar(Graphics &g,
                              const char *title,
                              uint16_t width,
                              uint32_t colour);
    static void paintIconPages(Graphics &g, uint16_t x, uint16_t y);

    // Delegated functions ---------------------------------------------------
    void setActivePage(uint8_t newActivePage);

    uint8_t activePage;
    UiDelegate *delegate;

    static constexpr uint8_t topPadding = 50;
};