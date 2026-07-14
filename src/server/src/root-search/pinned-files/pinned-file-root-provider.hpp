#pragma once
#include "config/config.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/action-pannel/action.hpp"
#include <filesystem>
#include <string>

// Provider "namespace" for pinned files. A favorite stored as
// "pinned-file:<absolute path>" is resurfaced as a root item by this provider,
// which lets arbitrary files be pinned into the Favorites section. The path is
// stored verbatim: if it later becomes invalid, opening it simply fails.
inline constexpr const char *PINNED_FILE_PROVIDER = "pinned-file";

class PinnedFileRootItem : public RootItem {
  std::string m_path;

  EntrypointId uniqueId() const override;
  QString title() const override;
  QString subtitle() const override;
  ImageURL iconUrl() const override;
  QString typeDisplayName() const override;
  AccessoryList accessories() const override;
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override;

public:
  explicit PinnedFileRootItem(std::string path) : m_path(std::move(path)) {}
};

class PinnedFileRootProvider : public RootProvider {
  config::Manager &m_config;

public:
  QString uniqueId() const override;
  QString displayName() const override;
  ImageURL icon() const override;
  Type type() const override;
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;

  explicit PinnedFileRootProvider(config::Manager &config);
};

// Pins (or unpins) a file by toggling its "pinned-file:<path>" favorite entry.
class PinFileAction : public AbstractAction {
  std::filesystem::path m_path;
  bool m_pin;

  void execute(ApplicationContext *ctx) override;

public:
  explicit PinFileAction(std::filesystem::path path, bool pin = true);
};
