import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import './style.css'
import BaseAwareLink from './BaseAwareLink.vue'
import SectionIndex from './SectionIndex.vue'
import Citation from './Citation.vue'
import ComplexityBadge from './ComplexityBadge.vue'
import DiagramCanvas from './DiagramCanvas.vue'

export default {
  extends: DefaultTheme,
  enhanceApp({ app }) {
    app.component('BaseAwareLink', BaseAwareLink)
    app.component('SectionIndex', SectionIndex)
    app.component('Citation', Citation)
    app.component('ComplexityBadge', ComplexityBadge)
    app.component('DiagramCanvas', DiagramCanvas)
  },
} satisfies Theme
