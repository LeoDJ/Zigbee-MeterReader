// put this file in zigbee2mqtt data directory (/usr/share/hassio/share/zigbee2mqtt for Home Assistant Supervised)
// and add the following lines to your zigbee2mqtt config:
// external_converters:
//  - metersensor_converter.js

const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const e = exposes.presets;

const tzLocal = {
    energy_set: {
        key: ['energy'],
        convertSet: async (entity, key, value, meta) => {
            const multiplier = entity.getClusterAttributeValue('seMetering', 'multiplier');
            const divisor = entity.getClusterAttributeValue('seMetering', 'divisor');
            const factor = multiplier && divisor ? multiplier / divisor : 1;

            let rawValue = value / factor;

            // for some reason zigbee2mqtt doesn't convert uint48 values automatically to the format the buffer writer needs (array of high 16 and low 32 bits)
            let toSend = [
                Math.floor(rawValue / 0x000100000000) & 0xFFFF, // upper 16 bits
                rawValue & 0x0000FFFFFFFF];                     // lower 32 bits

            // meta.logger.info(JSON.stringify({raw: rawValue, toSend: toSend}));

            await entity.write('seMetering', {'currentSummDelivered': toSend});
        }
    }
}

const device = {
    zigbeeModel: ['Pulsesensor'],
    model: 'Pulsesensor',
    vendor: 'Gingerlabs',
    description: 'Universal pulse sensor',
    fromZigbee: [fz.battery, fz.metering],
    toZigbee: [tzLocal.energy_set],
    meta: {configureKey: 1},
    exposes: [
        e.battery(), 
        e.power(), 
        e.energy()
            .withAccess(exposes.access.STATE_SET)
            .withDescription('Sum of consumed energy. Changing this field will instantly send an update to the Zigbee device once the input field loses focus, so be careful.')
    ],
    configure: async (device, coordinatorEndpoint, logger) => {
        const endpoint = device.getEndpoint(10);
        const binds = ['genPowerCfg', 'seMetering'];
        await reporting.bind(endpoint, coordinatorEndpoint, binds);
        await reporting.batteryPercentageRemaining(endpoint);
        await reporting.batteryVoltage(endpoint);
        await reporting.readMeteringMultiplierDivisor(endpoint);
        await reporting.currentSummDelivered(endpoint, {change: 0.001});
        await reporting.instantaneousDemand(endpoint, {min: 0, max: 900, change: 1}); // min max interval in s
    },
};

module.exports = device;