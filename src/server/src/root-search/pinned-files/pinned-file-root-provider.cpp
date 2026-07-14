#include "root-search/pinned-files/pinned-file-root-provider.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "common/entrypoint.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"
#include "utils/file-list-item.hpp"
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

EntrypointId PinnedFileRootItem::uniqueId() const { return EntrypointId{PINNED_FILE_PROVIDER, m_path}; }

QString PinnedFileRootItem::title() const {
  return QString::fromStdString(fs::path(m_path).filename().string());
}

QString PinnedFileRootItem::subtitle() const {
  return QString::fromStdString(fs::path(m_path).parent_path().string());
}

ImageURL PinnedFileRootItem::iconUrl() const { return ImageURL::fileIcon(fs::path(m_path)); }

QString PinnedFileRootItem::typeDisplayName() const { return "Pinned File"; }

AccessoryList PinnedFileRootItem::accessories() const {
  return {{.text = "Pinned File", .color = SemanticColor::TextMuted}};
}

std::unique_ptr<ActionPanelState>
PinnedFileRootItem::newActionPanel(ApplicationContext *ctx, const RootItemMetadata &metadata) const {
  // Reuse the standard file action panel (open / reveal / copy) and append the
  // favorite toggle, which for a pinned file reads "Remove from favorites" and
  // effectively unpins it (the provider only emits currently-pinned files).
  auto panel = FileActions::actionPanel(fs::path(m_path), ctx);
  auto section = panel->createSection();
  section->addAction(new ToggleItemAsFavorite(uniqueId(), metadata.favorite));
  return panel;
}

PinnedFileRootProvider::PinnedFileRootProvider(config::Manager &config) : m_config(config) {
  connect(&config, &config::Manager::configChanged, this,
          [this](const config::ConfigValue &, const config::ConfigValue &) { emit itemsChanged(); });
}

QString PinnedFileRootProvider::uniqueId() const { return PINNED_FILE_PROVIDER; }
QString PinnedFileRootProvider::displayName() const { return "Pinned Files"; }
ImageURL PinnedFileRootProvider::icon() const { return ImageURL::builtin("star"); }
RootProvider::Type PinnedFileRootProvider::type() const { return RootProvider::Type::GroupProvider; }

std::vector<std::shared_ptr<RootItem>> PinnedFileRootProvider::loadItems() const {
  std::vector<std::shared_ptr<RootItem>> items;

  for (const auto &fav : m_config.value().favorites) {
    auto id = EntrypointId::fromSerialized(fav);
    if (id.provider == PINNED_FILE_PROVIDER && !id.entrypoint.empty()) {
      items.emplace_back(std::make_shared<PinnedFileRootItem>(id.entrypoint));
    }
  }

  return items;
}

PinFileAction::PinFileAction(std::filesystem::path path, bool pin)
    : AbstractAction(pin ? "Pin to Launcher" : "Unpin from Launcher",
                     ImageURL::builtin(pin ? "star" : "star-disabled")),
      m_path(std::move(path)), m_pin(pin) {}

void PinFileAction::execute(ApplicationContext *ctx) {
  EntrypointId const id{PINNED_FILE_PROVIDER, m_path.string()};
  ctx->services->rootItemManager()->setItemAsFavorite(id, m_pin);
  ctx->navigation->showHud(m_pin ? "Pinned to launcher" : "Unpinned from launcher");
}
