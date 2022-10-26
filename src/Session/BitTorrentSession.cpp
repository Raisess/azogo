#include <iostream>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/session_params.hpp>
#include "BitTorrentSession.h"

AG::BitTorrentSession::BitTorrentSession() : Session<BitTorrentDownload>() {
  lt::settings_pack lt_settings;
  lt_settings.set_int(
    lt::settings_pack::alert_mask,
    lt::alert_category::error
    | lt::alert_category::status
    | lt::alert_category::storage
  );
  this->lt_session = std::make_unique<lt::session>(lt_settings);

  if (!this->lt_session) {
    std::cerr << "Error creating BitTorrentSession" << std::endl;
    exit(1);
  }
}

AG::BitTorrentSession::~BitTorrentSession() {}

void AG::BitTorrentSession::handle(const std::shared_ptr<BitTorrentDownload>& torrent) {
  if (torrent->state.status == DownloadState::Status::CREATED) {
    torrent->state.status = DownloadState::Status::ACTIVE;
    this->lt_session->add_torrent(torrent->get_lt_params());
  }

  try {
    std::vector<lt::alert*> lt_alerts;
    this->lt_session->pop_alerts(&lt_alerts);

    for (auto alert : lt_alerts) {
      if (lt::alert_cast<lt::torrent_error_alert>(alert)) {
        break;
      }

      auto state = lt::alert_cast<lt::state_update_alert>(alert);
      if (state) {
        if (state->status.empty()) continue;

        switch (torrent->state.status) {
          case DownloadState::Status::ACTIVE:
            {
              auto status = state->status[0];
              if (status.is_finished) {
                torrent->state.status = DownloadState::Status::FINISHED;
              }

              torrent->state.progress = status.progress_ppm / 10000;
              torrent->state.total_downloaded = status.total_done / 1000;
              torrent->state.download_rate = status.download_payload_rate / 1000;
              torrent->state.peers = status.num_peers;
            }
            break;
          case DownloadState::Status::FINISHED:
            {
              auto status = state->status[0];
              if (status.is_seeding) {
                torrent->state.status = DownloadState::Status::SEEDING;
              }
            }
            break;
          // TODO: Sedding and paused state status update
          case DownloadState::Status::SEEDING:
          case DownloadState::Status::PAUSED:
            break;
          // Do nothing states
          case DownloadState::Status::CREATED:
          case DownloadState::Status::FAILED:
            break;
        }
      }
    }

    this->lt_session->post_torrent_updates();
    return;
  } catch (std::exception& err) {
    std::cerr << "Error: " << err.what() << std::endl;
    return;
  }
}
