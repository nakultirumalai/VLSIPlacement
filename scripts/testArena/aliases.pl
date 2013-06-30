#!/usr/bin/perl -w

our %optionMap = (
    '-cluster_strategy' => {
	'bestchoice' => 'A1',
	'netcluster' => 'A2',
	'tdcluster1' => 'A3',
	'tdcluster2' => 'A4',
	'tdcluster3' => 'A5',
    },
    '-clust_max_area' => {
	'MA' => 'BX',
    },
    '-clust_max_cells' => {
	'MC' => 'CX',
    },
    '-global_placer' => {
	'ourPlacer' => 'D1',
	'ntuplace' => 'D2',
	'mpl6' => 'D3',
	'fastplace' => 'D4',
    },
    '-solver' => {
	'mosek' => 'E1',
	'conjgrad' => 'E2',
    },
    '-fvm' => {
	'td' => 'F1',
	'rql'=> 'F2',
	'connectivity' => 'F3',
	'custom1' => 'F4',
	'custom2' => 'F5',
    },
    '-vary_bin_size' => {
	'VBS' => 'G1',
    },
    '-intra_cluster_place' => {
	'center' => 'H1',
	'boundary' => 'H2',
    },
    '-uncluster' => {
	'legal' => 'I1',
	'nolegal' => 'I2',
    },
    '-shape' => {
	'SS' => 'J1',
    },
    'inter_clust_cell_move' => {
	'ICM' => 'K1',
    },
    -'legalizer' => {
	'basicDP' => 'L1',
	'bin_based' => 'L2',
	'bin_based_wc' => 'L3',
	'bin_based_tc' => 'L4',
	'bin_based_qc' => 'L5',
	'bin_based_lc' => 'L6',
	'fastplace' => 'L7',
    },
);

1;

