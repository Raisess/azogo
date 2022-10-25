#include <iostream>
#include <memory>
#include "Download/BitTorrentDownload.h"
#include "Session/BitTorrentSession.h"

#define OUTPUT_DIR "./downloads"

int main(int argc, char* argv[]) {
  auto torrent_list = {
    std::make_shared<AG::BitTorrentDownload>(
      "Sintel - Open Movie",
      "magnet:?xt=urn:btih:08ada5a7a6183aae1e09d831df6748d566095a10&dn=Sintel&tr=udp%3A%2F%2Fexplodie.org%3A6969&tr=udp%3A%2F%2Ftracker.coppersurfer.tk%3A6969&tr=udp%3A%2F%2Ftracker.empire-js.us%3A1337&tr=udp%3A%2F%2Ftracker.leechers-paradise.org%3A6969&tr=udp%3A%2F%2Ftracker.opentrackr.org%3A1337&tr=wss%3A%2F%2Ftracker.btorrent.xyz&tr=wss%3A%2F%2Ftracker.fastcast.nz&tr=wss%3A%2F%2Ftracker.openwebtorrent.com&ws=https%3A%2F%2Fwebtorrent.io%2Ftorrents%2F&xs=https%3A%2F%2Fwebtorrent.io%2Ftorrents%2Fsintel.torrent",
      OUTPUT_DIR
    ),
    std::make_shared<AG::BitTorrentDownload>(
      "Leaves of Grass by Walt Whitman",
      "magnet:?xt=urn:btih:d2474e86c95b19b8bcfdb92bc12c9d44667cfa36&dn=Leaves+of+Grass+by+Walt+Whitman.epub&tr=udp%3A%2F%2Ftracker.example4.com%3A80&tr=udp%3A%2F%2Ftracker.example5.com%3A80&tr=udp%3A%2F%2Ftracker.example3.com%3A6969&tr=udp%3A%2F%2Ftracker.example2.com%3A80&tr=udp%3A%2F%2Ftracker.example1.com%3A1337",
      OUTPUT_DIR
    )
  };

  AG::BitTorrentSession bt_session;
  bt_session.push_download(torrent_list);

  while (true) {
    std::system("clear");
    bt_session.handle([&]() {
      for (auto torrent : torrent_list) {
        std::cout << "Torrent: " << torrent->get_alias() << std::endl;
        std::cout << "--> Status: " << torrent->state.status << std::endl;
        std::cout << "--> Peers: " << torrent->state.peers << std::endl;
        std::cout << "--> Progress: " << unsigned(torrent->state.progress) << "%" << std::endl;
        std::cout << "--> Downloaded: " << unsigned(torrent->state.total_downloaded) << " kB" << std::endl;
        std::cout << "--> Download rate: " << unsigned(torrent->state.download_rate) << " kB/s" << std::endl;
        std::cout << "---------------------------------" << std::endl;
      }
    });
  }

  return 0;
}
