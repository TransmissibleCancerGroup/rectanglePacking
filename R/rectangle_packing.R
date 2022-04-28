#' @importFrom "data.table" `:=`
#' @export
rectangle_packing_cpp <- function(dt) {
    stopifnot("START" %in% colnames(dt))
    stopifnot("END" %in% colnames(dt))
    stopifnot("ID" %in% colnames(dt))
    
    dt[, layer := NA_integer_]
    layer_n <- 1
    while (dt[is.na(layer), .N] > 0) {
        ids = assign_to_layer(dt[is.na(layer)])
        dt[ID %in% ids, layer := layer_n]
        layer_n <- layer_n + 1
    }
}
