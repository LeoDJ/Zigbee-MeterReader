// put this file in zigbee2mqtt data directory (/usr/share/hassio/share/zigbee2mqtt for Home Assistant Supervised)
// and add the following lines to your zigbee2mqtt config:
// external_converters:
//  - metersensor_converter.js

const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const e = exposes.presets;

const device = {
    zigbeeModel: ['Pulsesensor'],
    model: 'Pulsesensor',
    vendor: 'Gingerlabs',
    description: 'Universal pulse sensor',
    fromZigbee: [fz.battery, fz.metering],
    toZigbee: [],
    meta: {configureKey: 1},
    exposes: [e.battery(), e.power(), e.energy()],
    configure: async (device, coordinatorEndpoint, logger) => {
        const endpoint = device.getEndpoint(10);
        const binds = ['genPowerCfg', 'seMetering'];
        await reporting.bind(endpoint, coordinatorEndpoint, binds);
        await reporting.batteryPercentageRemaining(endpoint);
        await reporting.batteryVoltage(endpoint);
        await reporting.readMeteringMultiplierDivisor(endpoint);
        await reporting.currentSummDelivered(endpoint);
        await reporting.instantaneousDemand(endpoint, {min: 0, max: 900, change: 1}); // min max interval in s
    },
};

module.exports = device;