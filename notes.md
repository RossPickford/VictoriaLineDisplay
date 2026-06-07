### Notes

## Hardware

[LED 64x32 Panel](https://www.aliexpress.com/item/1641364020.html?spm=a2g0o.productlist.main.2.67581e7529ZiEO&algo_pvid=959c482b-d9e0-4408-91c3-fb04c398dbb8&algo_exp_id=959c482b-d9e0-4408-91c3-fb04c398dbb8-1&pdp_ext_f=%7B%22order%22%3A%2244%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21GBP%219.99%219.99%21%21%2113.02%2113.02%21%402103867617808719017192515eb10d%2153628021726%21sea%21UK%210%21ABX%211%210%21n_tag%3A-29910%3Bd%3A448c2d59%3Bm03_new_user%3A-29895&curPageLogUid=I8kDROHlbfQd&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1641364020%7C_p_origin_prod%3A)

[LED Matrix Bonnet](https://www.aliexpress.com/item/1005011559056821.html?spm=a2g0o.productlist.main.2.7174VgFsVgFsfl&algo_pvid=71c0fc05-d89e-426e-a5ad-40e05c59eaca&algo_exp_id=71c0fc05-d89e-426e-a5ad-40e05c59eaca-1&pdp_ext_f=%7B%22order%22%3A%2216%22%2C%22spu_best_type%22%3A%22price%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21GBP%2110.50%218.93%21%21%2192.72%2178.81%21%402103890117808720174014629e79fd%2112000055919591176%21sea%21UK%210%21ABX%211%210%21n_tag%3A-29910%3Bd%3A448c2d59%3Bm03_new_user%3A-29895&curPageLogUid=AEchK23Dxu3E&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005011559056821%7C_p_origin_prod%3A)


## Software

[LED Matrix API](https://github.com/hzeller/rpi-rgb-led-matrix) - the library is written in C++, but there is a wrapper API in C

Parsing JSON file - need to utilise arenas to store the Train structs as they come in. 

create a buffer that is the size of each data id needed (e.g. current location) and then collect up to that in each quotation until found - then can parse until the next quotation for the result.